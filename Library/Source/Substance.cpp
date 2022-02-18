#include "../Header/Substance.h"



namespace StartingOver
{

	void CalculateNormalVector(std::vector<Vertex>& vertices, const std::vector<size_t>& indices);
	void CalculateTangentVector(std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices);


	void FetchAnimationTakes(FbxScene* scene, std::vector<AnimationTake>& animationTakes, size_t samplingRate)
	{
		FbxArray<FbxString*> animationStackNames;
		scene->FillAnimStackNameArray(animationStackNames);
		const size_t animationStackNum = animationStackNames.GetCount();
		for (size_t animationStackIndex = 0; animationStackIndex < animationStackNum; ++animationStackIndex)
		{
			AnimationTake animationTake;
			animationTake.name = animationStackNames[animationStackIndex]->Buffer();

			FbxAnimStack* animationStack = scene->FindMember<FbxAnimStack>(animationStackNames[animationStackIndex]->Buffer());
			_ASSERT_EXPR_A(animationStack, "This is a problem. The anim stack should be found in the scene!");
			scene->SetCurrentAnimationStack(animationStack);

			const FbxTime::EMode timeMode = scene->GetGlobalSettings().GetTimeMode();
			FbxTime frameTime;
			frameTime.SetTime(0, 0, 0, 1, 0, timeMode);
			if (samplingRate <= 0)
			{
				samplingRate = static_cast<size_t>(frameTime.GetFrameRate(timeMode));
			}

			animationTake.samplingRate = samplingRate;

			FbxTime samplingStep;
			samplingStep.SetTime(0, 0, 1, 0, 0, timeMode);
			samplingStep = static_cast<FbxLongLong>(samplingStep.Get() * (1.0f / samplingRate));

			FbxTakeInfo* take_info = scene->GetTakeInfo(*(animationStackNames[animationStackIndex]));
			FbxTime start_time;
			FbxTime stop_time;
			if (take_info)
			{
				start_time = take_info->mLocalTimeSpan.GetStart();
				stop_time = take_info->mLocalTimeSpan.GetStop();
			}
			else
			{
				FbxTimeSpan time_line_time_span;
				scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(time_line_time_span);
				start_time = time_line_time_span.GetStart();
				stop_time = time_line_time_span.GetStop();
			}
			animationTake.keyframeSize = static_cast<size_t>(((stop_time - start_time) / samplingStep).Get()) + 1;
			animationTakes.push_back(animationTake);
		}

		for (size_t animationStackIndex = 0; animationStackIndex < animationStackNum; ++animationStackIndex)
		{
			delete animationStackNames[animationStackIndex];
		}
	}

	struct BoneInfluence
	{
		std::uint32_t index;
		float weight;
	};

	struct BoneInfluencesPerControlPoint : public std::vector<BoneInfluence>
	{
		float totalWight = 0;
	};

	void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& influences)
	{
		const size_t controlPointsNum = fbxMesh->GetControlPointsCount();
		influences.resize(controlPointsNum);

		const size_t deformarNum = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (size_t deformarIndex = 0; deformarIndex < deformarNum; ++deformarIndex)
		{
			const auto skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformarIndex, FbxDeformer::eSkin));
			const size_t clusterNum = skin->GetClusterCount();
			for (size_t clusterIndex = 0; clusterIndex < clusterNum; ++clusterIndex)
			{
				const FbxCluster* cluster = skin->GetCluster(clusterIndex);

				const size_t controlPointIndicesNum = cluster->GetControlPointIndicesCount();
				const int* controlPointIndices = cluster->GetControlPointIndices();
				const double* controlPointWeights = cluster->GetControlPointWeights();

				for (size_t controlPointIndex = 0; controlPointIndex < controlPointIndicesNum; ++controlPointIndex)
				{
					BoneInfluencesPerControlPoint& influencesPerControlPoint = influences.at(controlPointIndices[controlPointIndex]);
					BoneInfluence influence;
					influence.index = clusterIndex;
					influence.weight = static_cast<float>(controlPointWeights[controlPointIndex]);
					influencesPerControlPoint.push_back(influence);
					influencesPerControlPoint.totalWight += influence.weight;

				}
			}
		}
	}

	void SetBoneInfluencesToVertex(Vertex& vertex, const BoneInfluencesPerControlPoint& influencesPerControlPoint)
	{
		for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex)
		{
			if (influenceIndex < MAXBONEINFLUENCE)
			{
				vertex.boneWeight[influenceIndex] = influencesPerControlPoint.at(influenceIndex).weight / influencesPerControlPoint.totalWight;
				vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).index;
			}
			else
			{
				size_t minValueIndex = 0;
				float minValue = FLT_MAX;
				for (size_t i = 0; i < MAXBONEINFLUENCE; ++i)
				{
					if (minValue > vertex.boneWeight[i])
					{
						minValue = vertex.boneWeight[i];
						minValueIndex = i;
					}
				}
				vertex.boneWeight[minValueIndex] += influencesPerControlPoint.at(influenceIndex).weight / influencesPerControlPoint.totalWight;
				vertex.boneIndices[minValueIndex] = influencesPerControlPoint.at(influenceIndex).index;

			}
		}
	}

	void FetchOffsetMatrices(const FbxMesh* fbxMesh, std::vector<Bone>& offsetTransforms)
	{
		const size_t deformersNum = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (size_t deformersIndex = 0; deformersIndex < deformersNum; ++deformersIndex)
		{
			const auto skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformersIndex, FbxDeformer::eSkin));
			const FbxSkin::EType skinningType = skin->GetSkinningType();
			_ASSERT_EXPR(skinningType == FbxSkin::eLinear || skinningType == FbxSkin::eRigid, L"Only support FbxSkin::eLinear or FbxSkin::eRigid");

			const FbxCluster::ELinkMode linkMode = skin->GetCluster(0)->GetLinkMode();
			_ASSERT_EXPR(linkMode != FbxCluster::eAdditive, L"Only support FbxCluster::eNormalize or FbxCluster::eTotalOne");

			const size_t clusterNum = skin->GetClusterCount();
			offsetTransforms.resize(clusterNum);
			for (size_t clusterIndex = 0; clusterIndex < clusterNum; ++clusterIndex)
			{
				Bone& bone = offsetTransforms.at(clusterIndex);

				FbxCluster* cluster = skin->GetCluster(clusterIndex);
				bone.name = cluster->GetLink()->GetName();

				// 'reference_global_init_position' trnasforms from model space to global space
				FbxAMatrix referenceGlobalInitPosition;
				cluster->GetTransformMatrix(referenceGlobalInitPosition);

				// 'cluster_global_init_position' trnasforms from bone space to global space
				FbxAMatrix clusterGlobalInitPosition;
				cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

				// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation (translation, rotation and scale), the last row of the matrix represents the translation part of the transformation.
				// make 'bone.transform' that trnasforms from model space to bone space (offset matrix)
				FbxAMatrixToMatrix(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition, bone.transform);

				// 'bone.transform_to_parent' is noe used in offset transforms
				bone.transformToParent = Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

			}
		}

	}

	void FetchKeyFrameMatrices(const FbxMesh* fbxMesh, Matrix transformKeyframe, const FbxTime time)
	{
		const FbxAMatrix referenceGlobalCurrentPosition = fbxMesh->GetNode()->EvaluateGlobalTransform(time);
		FbxAMatrixToMatrix(referenceGlobalCurrentPosition, transformKeyframe);
	}

	void FetchBoneMatrices(FbxMesh* fbxMesh, Skeletal& skeletal, FbxTime time)
	{
		const size_t deformersNum = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (size_t deformerIndex = 0; deformerIndex < deformersNum; ++deformerIndex)
		{
			const auto skin = dynamic_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

			const FbxSkin::EType skinningType = skin->GetSkinningType();
			_ASSERT_EXPR(skinningType == FbxSkin::eLinear || skinningType == FbxSkin::eRigid, L"only support FbxSkin::eLinear or FbxSkin::eRigid");

			const FbxCluster::ELinkMode linkMode = skin->GetCluster(0)->GetLinkMode();
			_ASSERT_EXPR(linkMode != FbxCluster::eAdditive, L"only support FbxCluster::eNormalize or FbxCluster::eTotalOne");

			const size_t clustersNum = skin->GetClusterCount();
			skeletal.bones.resize(clustersNum);
			for (size_t indexOfCluster = 0; indexOfCluster < clustersNum; ++indexOfCluster)
			{
				Bone& bone = skeletal.bones.at(indexOfCluster);

				FbxCluster* cluster = skin->GetCluster(indexOfCluster);
				bone.name = cluster->GetLink()->GetName();

				// 'cluster_global_current_position' trnasforms from bone space to global space
				FbxAMatrix clusterGlobalCurrentPosition;
				clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(time);
				FbxAMatrixToMatrix(clusterGlobalCurrentPosition, bone.transform);

				// make 'bone.transform_to_parent' that trnasforms from bone space to parent bone space
				bone.transformToParent = Matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
				if (FbxNode* parentNode = cluster->GetLink()->GetParent())
				{
					FbxAMatrix parentClusterGlobalCurrentPosition;
					parentClusterGlobalCurrentPosition = parentNode->EvaluateGlobalTransform(time);
					FbxAMatrixToMatrix(parentClusterGlobalCurrentPosition.Inverse() * clusterGlobalCurrentPosition, bone.transformToParent);
				}
			}
		}
	}

	void FetchAnimation(FbxMesh* fbxMesh, const AnimationTake& animationTake, Animation& animation)
	{
		_ASSERT_EXPR_A(animationTake.samplingRate > 0, "'animation_take.sampling_rate' is an invalid value.");

		FbxScene* fbxScene = fbxMesh->GetScene();
		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationTake.name.c_str());
		fbxScene->SetCurrentAnimationStack(animationStack);

		FbxTime startTime;
		FbxTime stopTime;
		if (const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationTake.name.c_str()))
		{
			startTime = takeInfo->mLocalTimeSpan.GetStart();
			stopTime = takeInfo->mLocalTimeSpan.GetStop();
		}
		else
		{
			FbxTimeSpan timeLineTimeSpan;
			fbxScene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeLineTimeSpan);
			startTime = timeLineTimeSpan.GetStart();
			stopTime = timeLineTimeSpan.GetStop();
		}

		const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		FbxTime samplingStep;
		samplingStep.SetTime(0, 0, 1, 0, 0, timeMode);
		samplingStep = samplingStep.Get() * (1.0f / animationTake.samplingRate);

		for (FbxTime time = startTime; time < stopTime; time += samplingStep)
		{
			Skeletal skeletal;
			FetchBoneMatrices(fbxMesh, skeletal, time);
			animation.skeletalTransforms.push_back(skeletal);

			ActorTransform actorTransform;
			FetchKeyFrameMatrices(fbxMesh, actorTransform.transform, time);
			animation.actorTransforms.push_back(actorTransform);
		}
		_ASSERT_EXPR_A(animationTake.keyframeSize == animation.skeletalTransforms.size(), "There's no consistency");
		_ASSERT_EXPR_A(animationTake.keyframeSize == animation.actorTransforms.size(), "There's no consistency");
		animation.name = animationTake.name;
	}

	void BlendSkeletals(
		const HierarchyNode* parent,
		const std::vector<Bone>* skeletals[2],
		const float ratio,
		std::vector<Bone>* _blendedSkeletal)
	{
		for (const HierarchyNode& node : parent->children)
		{
			if (node.attribute == FbxNodeAttribute::eSkeleton)
			{
				size_t boneIndex;
				const size_t boneNum = skeletals[0]->size();
				for (boneIndex = 0; boneIndex < boneNum; boneIndex++)
				{
					if (node.name == skeletals[0]->at(boneIndex).name)
					{
						break;
					}
				}
				_ASSERT_EXPR_A(boneIndex < boneNum, "Not found the bone");

				const StartingOver::Bone* bones[2];
				bones[0] = &skeletals[0]->at(boneIndex);
				bones[1] = &skeletals[1]->at(boneIndex);

				Matrix parentTransform;
				DirectX::XMVECTOR Q[2];
				Q[0] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&bones[0]->transformToParent));
				Q[1] = DirectX::XMQuaternionRotationMatrix(DirectX::XMLoadFloat4x4(&bones[1]->transformToParent));
				DirectX::XMStoreFloat4x4(&parentTransform, DirectX::XMMatrixRotationQuaternion(DirectX::XMQuaternionNormalize(DirectX::XMQuaternionSlerp(Q[0], Q[1], ratio))));
				parentTransform.m[3][0] = bones[0]->transformToParent.m[3][0] * (1 - ratio) + bones[1]->transformToParent.m[3][0] * ratio;
				parentTransform.m[3][1] = bones[0]->transformToParent.m[3][1] * (1 - ratio) + bones[1]->transformToParent.m[3][1] * ratio;
				parentTransform.m[3][2] = bones[0]->transformToParent.m[3][1] * (1 - ratio) + bones[1]->transformToParent.m[3][2] * ratio;
				parentTransform.m[0][3] = 0;
				parentTransform.m[1][3] = 0;
				parentTransform.m[2][3] = 0;
				parentTransform.m[3][3] = 1;

				Matrix blendedTransform(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
				DirectX::XMStoreFloat4x4(&blendedTransform, DirectX::XMLoadFloat4x4(&parentTransform) * DirectX::XMLoadFloat4x4(&parentTransform));
				_blendedSkeletal->at(boneIndex).name = node.name;
				_blendedSkeletal->at(boneIndex).transform = blendedTransform;
				_blendedSkeletal->at(boneIndex).transformToParent = parentTransform;
				BlendSkeletals(&node, skeletals, ratio, _blendedSkeletal);
			}
			else
			{
				BlendSkeletals(&node, skeletals, ratio, _blendedSkeletal);
			}
		}

	}

	void FetchMaterials(const FbxNode* fbxNode, std::unordered_map<std::string, Material>& materials)
	{
		const size_t materialsNum = fbxNode->GetMaterialCount();
		for (size_t materialIndex = 0; materialIndex < materialsNum; ++materialIndex)
		{
			const FbxSurfaceMaterial* surfaceMaterial = fbxNode->GetMaterial(materialIndex);
			if (materials.find(surfaceMaterial->GetName()) != materials.end())
			{
				continue;
			}
			std::function<void(Material&, const std::string, const std::string, bool)> FetchMaterialProperty =
				[&surfaceMaterial](Material& material, const std::string propertyName, const std::string factorName, const bool forceSRGB)
			{
				Material::Property property;
				property.name = propertyName;
				property.forceSRGB = forceSRGB;

				const FbxProperty fbxProperty = surfaceMaterial->FindProperty(propertyName.c_str());
				const FbxProperty fbxFactor = surfaceMaterial->FindProperty(factorName.c_str());
				if(fbxProperty.IsValid())
				{
					property.color.x = static_cast<float>(fbxProperty.Get<FbxDouble3>()[0]);
					property.color.y = static_cast<float>(fbxProperty.Get<FbxDouble3>()[1]);
					property.color.z = static_cast<float>(fbxProperty.Get<FbxDouble3>()[2]);
					property.color.w = 1.0f;
					if(const int textureNum = fbxProperty.GetSrcObjectCount<FbxFileTexture>(); textureNum > 0)
					{
						if(const FbxFileTexture* fileTexture = fbxProperty.GetSrcObject<FbxFileTexture>())
						{
							property.textureFileName = fileTexture->GetFileName();
							property.textureFileName = fileTexture->GetRelativeFileName();

							property.uScale = static_cast<float>(fileTexture->GetScaleU());
							property.vScale = static_cast<float>(fileTexture->GetScaleV());
						}
					}
				}
				if(fbxFactor.IsValid())
				{
					property.factor = static_cast<float>(fbxFactor.Get<FbxDouble>());
				}
				material.properties.push_back(property);
			};

			Material material;
			material.name = surfaceMaterial->GetName();
			material.shadingModel = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sShadingModel).Get<FbxString>();
			material.multiLayer = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sMultiLayer).Get<FbxBool>();

			FetchMaterialProperty(material, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, true);
			FetchMaterialProperty(material, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, true);
			FetchMaterialProperty(material, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, false);

			const FbxDouble shininess = surfaceMaterial->FindProperty(FbxSurfaceMaterial::sShininess).Get<FbxDouble>();
			material.properties.at(2).color.w = static_cast<float>(shininess);

			FetchMaterialProperty(material, FbxSurfaceMaterial::sNormalMap, FbxSurfaceMaterial::sBumpFactor, false);
			material.properties.at(3).color.x = material.properties.at(3).color.y = 0.5f;
			material.properties.at(3).color.z = material.properties.at(3).color.w = 1.0f;
			FetchMaterialProperty(material, FbxSurfaceMaterial::sBump, FbxSurfaceMaterial::sBumpFactor, false);
			material.properties.at(4).color.x = material.properties.at(4).color.y = 0.5f;
			material.properties.at(4).color.z = material.properties.at(4).color.w = 1.0f;

			FetchMaterialProperty(material, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, false);
			FetchMaterialProperty(material, FbxSurfaceMaterial::sTransparentColor, FbxSurfaceMaterial::sTransparencyFactor, false);
			FetchMaterialProperty(material, FbxSurfaceMaterial::sReflection, FbxSurfaceMaterial::sReflectionFactor, false);
			FetchMaterialProperty(material, FbxSurfaceMaterial::sDisplacementColor, FbxSurfaceMaterial::sDisplacementFactor, false);
			FetchMaterialProperty(material, FbxSurfaceMaterial::sVectorDisplacementColor, FbxSurfaceMaterial::sVectorDisplacementFactor, false);

			materials.insert(std::make_pair(surfaceMaterial->GetName(), material));

		}
	}

	Vertex FetchVertex(FbxMesh* fbxMesh,FbxStringList& uvNames, const size_t polygonIndex, const size_t edgeIndex, const bool vCoordinatesFlip = true)
	{
		Vertex vertex;

		const bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
		const bool hasTangent = fbxMesh->GetElementTangentCount() > 0;
		const bool hasBiNormal = fbxMesh->GetElementBinormalCount() > 0;
		const bool hasUV = fbxMesh->GetElementUVCount() > 0;
		const bool hasVertexColor = fbxMesh->GetElementVertexColorCount() > 0;

		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();
		const size_t controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, edgeIndex);

		vertex.position.x = static_cast<float>(controlPoints[controlPointIndex][0]);
		vertex.position.y = static_cast<float>(controlPoints[controlPointIndex][1]);
		vertex.position.z = static_cast<float>(controlPoints[controlPointIndex][2]);

		if(hasNormal)
		{
#if 1
			FbxVector4 normal;
			fbxMesh->GetPolygonVertexNormal(polygonIndex, edgeIndex, normal);
			vertex.normal.x = static_cast<float>(normal[0]);
			vertex.normal.y = static_cast<float>(normal[1]);
			vertex.normal.z = static_cast<float>(normal[2]);
#else

			const FbxGeometryElementNormal* geometryElementNormal = _fbxMesh->GetElementNormal(0);
			FbxGeometryElement::EMappingMode mappingMode = geometryElementNormal->GetMappingMode();
			FbxGeometryElement::EReferenceMode referenceMode = geometryElementNormal->GetReferenceMode();

			if (mappingMode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = controlPointIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementNormal->GetIndexArray().GetAt(vertexIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.normal.x = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.normal.y = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.normal.z = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else if (mappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = _polygonIndex * 3 + _edgeIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementNormal->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.normal.x = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.normal.y = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.normal.z = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else
			{
				_ASSERT_EXPR(false, L"Invalid Mapping Mode");
				throw std::exception("Invalid Mapping Mode");
			}
#endif
		}

		if(hasTangent)
		{
			const FbxGeometryElementTangent* geometryElementTangent = fbxMesh->GetElementTangent(0);

			const FbxGeometryElement::EMappingMode mappingMode = geometryElementTangent->GetMappingMode();

			const FbxGeometryElement::EReferenceMode referenceMode = geometryElementTangent->GetReferenceMode();

			if (mappingMode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = controlPointIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementTangent->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.tangent.x = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.tangent.y = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.tangent.z = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[2]);
				vertex.tangent.w = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[3]);
			}
			else if (mappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = polygonIndex * 3 + edgeIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementTangent->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.tangent.x = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.tangent.y = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.tangent.z = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[2]);
				vertex.tangent.w = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[3]);
			}
			else
			{
				_ASSERT_EXPR(false, L"Invalid Mapping Mode");
				throw std::exception("Invalid Mapping Mode");
			}
		}

		if (hasBiNormal)
		{
			const FbxGeometryElementBinormal* geometryElementBiNormal = fbxMesh->GetElementBinormal(0);

			const FbxGeometryElement::EMappingMode mappingMode = geometryElementBiNormal->GetMappingMode();

			const FbxGeometryElement::EReferenceMode referenceMode = geometryElementBiNormal->GetReferenceMode();

			if (mappingMode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = controlPointIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementBiNormal->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.biNormal.x = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.biNormal.y = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.biNormal.z = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else if (mappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				size_t indexOfVertex;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					indexOfVertex = polygonIndex * 3 + edgeIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					indexOfVertex = geometryElementBiNormal->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.biNormal.x = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(indexOfVertex)[0]);
				vertex.biNormal.y = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(indexOfVertex)[1]);
				vertex.biNormal.z = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(indexOfVertex)[2]);
			}
			else
			{
				_ASSERT_EXPR(false, L"Invalid Mapping Mode");
				throw std::exception("Invalid Mapping Mode");
			}
		}
		if (hasUV)
		{
#if 1
			FbxVector2 uv;
			bool unmappedUV;
			fbxMesh->GetPolygonVertexUV(polygonIndex, edgeIndex, uvNames[0], uv, unmappedUV);
			vertex.texCoord.x = static_cast<float>(uv[0]);
			vertex.texCoord.y = static_cast<float>(uv[1]);
#else
			const FbxGeometryElementUV* geometryElementUV = _fbxMesh->GetElementUV(0);
			FbxGeometryElement::EMappingMode mappingMode = geometryElementUV->GetMappingMode();
			FbxGeometryElement::EReferenceMode referenceMode = geometryElementUV->GetReferenceMode();
			if (mappingMode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = controlPointIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementUV->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.texCoord.y = static_cast<float>(geometryElementUV->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.texCoord.x = static_cast<float>(geometryElementUV->GetDirectArray().GetAt(vertexIndex)[0]);
			}
			else if (mappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = _polygonIndex * 3 + _edgeIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementUV->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.texCoord.x = static_cast<float>(geometryElementUV->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.texCoord.y = static_cast<float>(geometryElementUV->GetDirectArray().GetAt(vertexIndex)[1]);
			}
			else
			{
				_ASSERT_EXPR(false, L"Invalid Mapping Mode");
				throw std::exception("Invalid Mapping Mode");
			}
#endif
			if (vCoordinatesFlip)
			{
				vertex.texCoord.y = 1 - vertex.texCoord.y;
			}
		}
		if (hasVertexColor)
		{
			const FbxGeometryElementVertexColor* geometryElementVertexColor = fbxMesh->GetElementVertexColor(0);
			const FbxGeometryElement::EMappingMode mappingMode = geometryElementVertexColor->GetMappingMode();
			const FbxGeometryElement::EReferenceMode referenceMode = geometryElementVertexColor->GetReferenceMode();
			if (mappingMode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = controlPointIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementVertexColor->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.color.x = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.color.y = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.color.z = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else if (mappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				size_t vertexIndex = 0;
				switch (referenceMode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					vertexIndex = polygonIndex * 3 + edgeIndex;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					vertexIndex = geometryElementVertexColor->GetIndexArray().GetAt(controlPointIndex);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.color.x = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.color.y = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.color.z = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else
			{
				_ASSERT_EXPR(false, L"Invalid Mapping Mode");
				throw std::exception("Invalid Mapping Mode");
			}
		}
		return vertex;
	}

	Vertex FetchVertexByControlPoint(FbxMesh* fbxMesh, const size_t controlPointIndex, const bool vCoordinatesFlip = true)
	{
		Vertex vertex;

		const bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
		const bool hasTangent = fbxMesh->GetElementTangentCount() > 0;
		const bool hasBiNormal = fbxMesh->GetElementBinormalCount() > 0;
		const bool hasUV = fbxMesh->GetElementUVCount() > 0;
		const bool hasVertexColor = fbxMesh->GetElementVertexColorCount() > 0;

		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();
		vertex.position.x = static_cast<float>(controlPoints[controlPointIndex][0]);
		vertex.position.y = static_cast<float>(controlPoints[controlPointIndex][1]);
		vertex.position.z = static_cast<float>(controlPoints[controlPointIndex][2]);

		if (hasNormal)
		{
			const FbxGeometryElementNormal* geometryElementNormal = fbxMesh->GetElementNormal(0);
			size_t vertexIndex = controlPointIndex;
			if (geometryElementNormal->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				vertexIndex = geometryElementNormal->GetIndexArray().GetAt(controlPointIndex);
			}
			vertex.normal.x = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[0]);
			vertex.normal.y = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[1]);
			vertex.normal.z = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[2]);
		}
		if (hasTangent)
		{
			const FbxGeometryElementTangent* geometryElementTangent = fbxMesh->GetElementTangent(0);
			size_t vertexIndex = controlPointIndex;
			if (geometryElementTangent->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				vertexIndex = geometryElementTangent->GetIndexArray().GetAt(controlPointIndex);
			}
			vertex.tangent.x = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(controlPointIndex)[0]);
			vertex.tangent.y = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(controlPointIndex)[1]);
			vertex.tangent.z = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(controlPointIndex)[2]);
			vertex.tangent.w = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(controlPointIndex)[3]);
		}
		if (hasBiNormal)
		{
			const FbxGeometryElementBinormal* geometryElementBiNormal = fbxMesh->GetElementBinormal(0);
			size_t vertexIndex = controlPointIndex;
			if (geometryElementBiNormal->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				vertexIndex = geometryElementBiNormal->GetIndexArray().GetAt(controlPointIndex);
			}
			vertex.biNormal.x = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(vertexIndex)[0]);
			vertex.biNormal.y = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(vertexIndex)[1]);
			vertex.biNormal.z = static_cast<float>(geometryElementBiNormal->GetDirectArray().GetAt(vertexIndex)[2]);
		}
		if (hasUV)
		{
			const FbxGeometryElementUV* geometryElementUv = fbxMesh->GetElementUV(0);
			size_t vertexIndex = controlPointIndex;
			if (geometryElementUv->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				vertexIndex = geometryElementUv->GetIndexArray().GetAt(controlPointIndex);
			}
			vertex.texCoord.x = static_cast<float>(geometryElementUv->GetDirectArray().GetAt(vertexIndex)[0]);
			vertex.texCoord.y = static_cast<float>(geometryElementUv->GetDirectArray().GetAt(vertexIndex)[1]);
			if (vCoordinatesFlip)
			{
				vertex.texCoord.y = 1 - vertex.texCoord.y;
			}
		}
		if (hasVertexColor)
		{
			const FbxGeometryElementVertexColor* geometryElementVertexColor = fbxMesh->GetElementVertexColor(0);
			size_t vertexIndex = controlPointIndex;
			if (geometryElementVertexColor->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				vertexIndex = geometryElementVertexColor->GetIndexArray().GetAt(controlPointIndex);
			}
			vertex.color.x = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[0]);
			vertex.color.y = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[1]);
			vertex.color.z = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[2]);
			vertex.color.w = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[3]);
		}
		return vertex;
	}

	void FetchVerticesAndIndices(FbxMesh* fbxMesh, std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices, std::vector<Subset>& subsets)
	{
		bool allByControlPoint = true;
		bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
		bool hasTangent = fbxMesh->GetElementTangentCount() > 0;
		bool hasBiNormal = fbxMesh->GetElementBinormalCount() > 0;
		bool hasUV = fbxMesh->GetElementUVCount() > 0;
		bool hasVertexColor = fbxMesh->GetElementVertexColorCount() > 0;
		if (hasTangent && fbxMesh->GetElementNormal(0)->GetMappingMode() != FbxGeometryElement::EMappingMode::eByControlPoint)
		{
			allByControlPoint = false;
		}
		if (hasTangent && fbxMesh->GetElementTangent(0)->GetMappingMode() != FbxGeometryElement::EMappingMode::eByControlPoint)
		{
			allByControlPoint = false;
		}
		if (hasBiNormal && fbxMesh->GetElementBinormal(0)->GetMappingMode() != FbxGeometryElement::EMappingMode::eByControlPoint)
		{
			allByControlPoint = false;
		}
		if (hasUV && fbxMesh->GetElementUV(0)->GetMappingMode() != FbxGeometryElement::EMappingMode::eByControlPoint)
		{
			allByControlPoint = false;
		}
		if (hasVertexColor && fbxMesh->GetElementVertexColor(0)->GetMappingMode() != FbxGeometryElement::EMappingMode::eByControlPoint)
		{
			allByControlPoint = false;
		}

		const size_t polygonNum = fbxMesh->GetPolygonCount();
		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();

		std::vector<BoneInfluencesPerControlPoint> boneInfluences;
		FetchBoneInfluences(fbxMesh, boneInfluences);

		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		// Fetch vertices
		if (allByControlPoint)
		{
			const size_t controlPointsNum = fbxMesh->GetControlPointsCount();
			for (size_t controlPointIndex = 0; controlPointIndex < controlPointsNum; ++controlPointIndex)
			{
				Vertex vertex;
				vertex = FetchVertexByControlPoint(fbxMesh,controlPointIndex);

				BoneInfluencesPerControlPoint influencesPerControlPoint = boneInfluences.at(controlPointIndex);
				SetBoneInfluencesToVertex(vertex, influencesPerControlPoint);

				vertices.push_back(vertex);
			}
		}
		else
		{
			for (size_t polygonIndex = 0; polygonIndex < polygonNum; polygonIndex++)
			{
				for (size_t edgeIndex = 0; edgeIndex < 3; edgeIndex++)
				{
					Vertex fetchedVertex;
					fetchedVertex = FetchVertex(fbxMesh, uvNames, polygonIndex, edgeIndex);

					const size_t indexOfControlPoint = fbxMesh->GetPolygonVertex(polygonIndex, edgeIndex);
					BoneInfluencesPerControlPoint influencesPerControlPoint = boneInfluences.at(indexOfControlPoint);
					SetBoneInfluencesToVertex(fetchedVertex, influencesPerControlPoint);

					vertices.push_back(fetchedVertex);
				}
			}
		}

		// Build subsets for each material
		const size_t materialsNum = fbxMesh->GetNode()->GetMaterialCount();

		if(materialsNum > 0)
		{
			subsets.resize(materialsNum);
		}
		else
		{
			subsets.resize(0);
		}

		for (size_t materialIndex = 0; materialIndex < materialsNum; ++materialIndex)
		{
			const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			subsets.at(materialIndex).name = surfaceMaterial->GetName();
		}

		if (materialsNum > 0)
		{
			// 各素材の面を数える
			const size_t polygonsNum = fbxMesh->GetPolygonCount();
			for (size_t polygonIndex = 0; polygonIndex < polygonsNum; ++polygonIndex)
			{
				const size_t index_of_material = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
				subsets.at(index_of_material).indexCount += 3;
			}

			// オフセットの記録
			size_t offset = 0;
			for (Subset& subset : subsets)
			{
				subset.indexStart = offset;
				offset += subset.indexCount;
				subset.indexCount = 0;
			}
		}

		size_t vertexCount = 0;
		indices.resize(polygonNum * 3);
		for (size_t polygonIndex = 0; polygonIndex < polygonNum; polygonIndex++)
		{
			// 現在の面のマテリアル
			size_t materialIndex;
			if (materialsNum > 0)
			{
				materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
			}
			else // fbxMeshにマテリアルがないときはダミーを作成
			{
				materialIndex = 0;
			}

			Subset& subset = subsets.at(materialIndex);
			const size_t indexOffset = subset.indexStart + subset.indexCount;

			for (size_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
			{
				const size_t controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, edgeIndex);
				if (allByControlPoint)
				{
					indices.at(indexOffset + edgeIndex) = static_cast<std::uint32_t>(controlPointIndex);
				}
				else
				{
					indices.at(indexOffset + edgeIndex) = vertexCount;
				}
				vertexCount += 1;
			}
			subset.indexCount += 3;
		}
		if (!hasNormal)
		{
			CalculateTangentVector(vertices, indices);
			hasNormal = true;
		}
		if (!hasTangent && hasUV && hasNormal)
		{
			CalculateTangentVector(vertices, indices);
			hasTangent = true;
		}
	}

	void CalculateTangentVector(std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
	{
		const size_t verticesNum = vertices.size();
		auto tan1 = new Vector3[verticesNum * 2];
		Vector3* tan2 = tan1 + verticesNum;
		std::fill_n(tan1, verticesNum * 2, Vector3(0, 0, 0));

		const size_t trianglesNum = indices.size() / 3;
		for (size_t triangleIndex = 0; triangleIndex < trianglesNum; triangleIndex++)
		{
			const size_t i1 = indices.at(triangleIndex * 3 + 0);
			const size_t i2 = indices.at(triangleIndex * 3 + 1);
			const size_t i3 = indices.at(triangleIndex * 3 + 2);

			const Vertex& v1 = vertices.at(i1);
			const Vertex& v2 = vertices.at(i2);
			const Vertex& v3 = vertices.at(i3);

			const float x1 = v2.position.x - v1.position.x;
			const float x2 = v3.position.x - v1.position.x;
			const float y1 = v2.position.y - v1.position.y;
			const float y2 = v3.position.y - v1.position.y;
			const float z1 = v2.position.z - v1.position.z;
			const float z2 = v3.position.z - v1.position.z;

			const float s2 = v3.texCoord.x - v1.texCoord.x;
			const float s1 = v2.texCoord.x - v1.texCoord.x;
			constexpr bool vCoordinatesFlipped = true;
			const float t1 = vCoordinatesFlipped ? (1 - v2.texCoord.y) - (1 - v1.texCoord.y) : v2.texCoord.y - v1.texCoord.y;
			const float t2 = vCoordinatesFlipped ? (1 - v3.texCoord.y) - (1 - v1.texCoord.y) : v3.texCoord.y - v1.texCoord.y;

			const float r = 1.0f / (s1 * t2 - s2 * t1);

			const Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			const Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

			tan1[i1].x += sdir.x;
			tan1[i1].y += sdir.y;
			tan1[i1].z += sdir.z;
			tan1[i2].x += sdir.x;
			tan1[i2].y += sdir.y;
			tan1[i2].z += sdir.z;
			tan1[i3].x += sdir.x;
			tan1[i3].y += sdir.y;
			tan1[i3].z += sdir.z;

			tan2[i1].x += tdir.x;
			tan2[i1].y += tdir.y;
			tan2[i1].z += tdir.z;
			tan2[i2].x += tdir.x;
			tan2[i2].y += tdir.y;
			tan2[i2].z += tdir.z;
			tan2[i3].x += tdir.x;
			tan2[i3].y += tdir.y;
			tan2[i3].z += tdir.z;
		}

		for (size_t vertexIndex = 0; vertexIndex < verticesNum; vertexIndex++)
		{
			Vertex& v = vertices.at(vertexIndex);

			const DirectX::XMVECTOR N = XMLoadFloat3(&v.normal);
			const DirectX::XMVECTOR T = XMLoadFloat3(&tan1[vertexIndex]);
			const DirectX::XMVECTOR B = XMLoadFloat3(&tan2[vertexIndex]);

			XMStoreFloat4(&v.tangent, DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(T, DirectX::XMVectorMultiply(N, DirectX::XMVector3Dot(T, N)))));
			v.tangent.w = DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Cross(N, T), B)) < 0.0f ? -1.0f : 1.0f;

			DirectX::XMStoreFloat3(&v.biNormal, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(N, T)));
		}

		delete[] tan1;
	}

}

