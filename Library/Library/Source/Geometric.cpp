#include "../Header/Geometric.h"




namespace StartingOver
{

	void CalculateNormalVector(std::vector<Geometric::Vertex>& vertices, const std::vector<unsigned int>& indices);
	void CalculateTangentVector(std::vector<Geometric::Vertex>& vertices, const std::vector<unsigned int>& indices);

	struct BoneInfluence
	{
		int index;
		float weight;
	};

	struct BoneInfluencesPerControlPoint : public std::vector<BoneInfluence>
	{
		float totalWeight = 0;
	};

	void FetchBoneInfluence(const FbxMesh* fbxMesh, std::vector<BoneInfluencesPerControlPoint>& influences)
	{
		const int controlPointsCount = fbxMesh->GetControlPointsCount();
		influences.resize(controlPointsCount);

		const int deformersCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int deformerIndex = 0; deformerIndex < deformersCount; ++deformerIndex)
		{
			auto* skin = dynamic_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

			const int clustersCount = skin->GetClusterCount();
			for (int clusterIndex = 0; clusterIndex < clustersCount; ++clusterIndex)
			{
				FbxCluster* cluster = skin->GetCluster(clusterIndex);

				const int controlPointIndicesCount = cluster->GetControlPointIndicesCount();
				const int* controlPointIndicesArray = cluster->GetControlPointIndices();
				const double* controlPointWeightsArray = cluster->GetControlPointWeights();

				for (int i = 0; i < controlPointIndicesCount; ++i)
				{
					BoneInfluencesPerControlPoint& boneInfluencesPerControlPoint = influences.at(controlPointIndicesArray[i]);
					BoneInfluence influence;
					influence.index = clusterIndex;
					influence.weight = static_cast<float>(controlPointWeightsArray[i]);
					boneInfluencesPerControlPoint.push_back(influence);
					boneInfluencesPerControlPoint.totalWeight += influence.weight;
				}
			}
		}

	}
	void SetBoneInfluencesToVertex(Geometric::Vertex& vertex, BoneInfluencesPerControlPoint& boneInfluencesPerControlPoint)
	{
		for (size_t influenceIndex = 0; influenceIndex < boneInfluencesPerControlPoint.size(); influenceIndex++)
		{
			if (influenceIndex < MAXBONEINFLUENCES)
			{
				vertex.boneWeights[influenceIndex] = boneInfluencesPerControlPoint.at(influenceIndex).weight / boneInfluencesPerControlPoint.totalWeight;
				vertex.boneIndices[influenceIndex] = boneInfluencesPerControlPoint.at(influenceIndex).index;
			}
			else
			{
#if 0
				for (int i = 0; i < MAXBONEINFLUENCES; i++)
				{
					if (vertex.boneWeights[i] < boneInfluencesPerControlPoint.at(influenceIndex).weight)
					{
						vertex.boneWeights[i] += boneInfluencesPerControlPoint.at(influenceIndex).weight / boneInfluencesPerControlPoint.totalWeight;
						vertex.boneIndices[i] = boneInfluencesPerControlPoint.at(influenceIndex).index;
						break;
					}
				}
#else
				int minIndex = 0;
				float minVelue = FLT_MAX;
				for(int i = 0;i < MAXBONEINFLUENCES;i++)
				{
					if(minVelue > vertex.boneWeights[i])
					{
						minVelue = vertex.boneWeights[i];
						minIndex = i;
					}
				}

				vertex.boneWeights[minIndex] += boneInfluencesPerControlPoint.at(influenceIndex).weight / boneInfluencesPerControlPoint.totalWeight;
				vertex.boneIndices[minIndex] = boneInfluencesPerControlPoint.at(influenceIndex).index;
#endif
			}
		}
	}

	void FetchBoneMatrices(FbxMesh* fbxMesh,Geometric::Skeletal& skeletal,FbxTime time)
	{
		const int deformersCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
		for (int deformersIndex = 0; deformersIndex < deformersCount; ++deformersIndex)
		{
			auto skin = dynamic_cast<FbxSkin*>(fbxMesh->GetDeformer(deformersIndex, FbxDeformer::eSkin));

			FbxSkin::EType skinningType = skin->GetSkinningType();
			_ASSERT_EXPR(skinningType == FbxSkin::eLinear || skinningType == FbxSkin::eRigid, L"only support FbxSkin::eLinear or FbxSkin::eRigid");

			FbxCluster::ELinkMode linkMode = skin->GetCluster(0)->GetLinkMode();
			_ASSERT_EXPR(linkMode != FbxCluster::eAdditive, L"only support FbxCluster::eNormalize or FbxCluster::eTotalOne");

			const int clustersCount = skin->GetClusterCount();
			skeletal.bones.resize(clustersCount);
			for(int clusterIndex = 0;clusterIndex < clustersCount;++clusterIndex)
			{
				Geometric::Bone& bone = skeletal.bones.at(clusterIndex);

				FbxCluster* cluster = skin->GetCluster(clusterIndex);

				FbxAMatrix referenceGlobalInitPosition;
				cluster->GetTransformMatrix(referenceGlobalInitPosition);

				FbxAMatrix clusterGlobalInitPosition;
				cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

				FbxAMatrix referenceGlobalCurrentPosition = fbxMesh->GetNode()->EvaluateGlobalTransform(time);

				FbxAMatrix clusterGlobalCurrentPosition = cluster->GetLink()->EvaluateGlobalTransform(time);

				FbxAMatrixToMatrix(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition, bone.offsetTransform);
				FbxAMatrixToMatrix(referenceGlobalCurrentPosition.Inverse() * clusterGlobalCurrentPosition, bone.transform);

				FbxNode* node = cluster->GetLink();
				bone.name = node->GetName();
				FbxAMatrix parentClusterGlobalCurrentPosition = node->GetParent()->EvaluateGlobalTransform(time);
				FbxAMatrixToMatrix(parentClusterGlobalCurrentPosition.Inverse() * clusterGlobalCurrentPosition, bone.parentTransform);
			}
		}
	}

	void Geometric::FetchAnimations(FbxMesh* fbxMesh, std::vector<Animation>& animations, int samplingRate)
	{
		// no animations
		if(samplingRate < 0)
		{
			return;
		}

		FbxArray<FbxString*> animationStackNamesArray;
		fbxMesh->GetScene()->FillAnimStackNameArray(animationStackNamesArray);

		const int animationsCount = animationStackNamesArray.Size();

		animations.resize(animationsCount);

		for (int animationIndex = 0; animationIndex < animationsCount; animationIndex++)
		{
			Animation& skeletalAnimation = animations.at(animationIndex);

			FbxTime::EMode timeMode = fbxMesh->GetScene()->GetGlobalSettings().GetTimeMode();
			FbxTime frameTime;
			frameTime.SetTime(0, 0, 0, 1, 0, timeMode);
			float samplingTime;
			if (samplingRate > 0)
			{
				samplingTime = samplingRate;
			}
			else
			{
				samplingTime = static_cast<unsigned int>(frameTime.GetFrameRate(timeMode));
			}
			skeletalAnimation.samplingTime = samplingTime;

			FbxString* animationStackName = animationStackNamesArray.GetAt(animationIndex);
			skeletalAnimation.name = animationStackName->Buffer();

			FbxAnimStack* currentAnimationStack = fbxMesh->GetScene()->FindMember<FbxAnimStack>(animationStackName->Buffer());
			fbxMesh->GetScene()->SetCurrentAnimationStack(currentAnimationStack);

			FbxTakeInfo* takeInfo = fbxMesh->GetScene()->GetTakeInfo(animationStackName->Buffer());
			_ASSERT_EXPR(takeInfo, L"must need FbxTakeInfo");
			FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime endTime = takeInfo->mLocalTimeSpan.GetStop();

			FbxTime samplingStep;
			samplingStep.SetTime(0, 0, 1, 0, 0, timeMode);
			samplingStep = static_cast<FbxLongLong>(samplingStep.Get() * samplingTime);
			for(FbxTime currentTime = startTime;currentTime < endTime;currentTime += samplingStep)
			{
				Skeletal skeletal;
				FetchBoneMatrices(fbxMesh, skeletal, currentTime);
				skeletalAnimation.skeletals.push_back(skeletal);
			}
		}
		for(int i = 0;i < animationsCount;i++)
		{
			delete animationStackNamesArray[i];
		}
	}

	void Geometric::FetchAnimationTakes(FbxScene* scene, std::vector<Geometric::AnimationTake>& animationTakes, int samplingRate)
	{
		// animation data not needed
		if(samplingRate < 0)
		{
			return;
		}

		FbxArray<FbxString*> animationStackNamesArray;
		scene->FillAnimStackNameArray(animationStackNamesArray);
		const int animationStacksCount = animationStackNamesArray.GetCount();
		for(int animationStackIndex = 0;animationStackIndex < animationStacksCount;animationStackIndex++)
		{
			Geometric::AnimationTake takeInfo;
			takeInfo.name = animationStackNamesArray[animationStackIndex]->Buffer();

			FbxAnimStack* currentAnimationStack = scene->FindMember<FbxAnimStack>(animationStackNamesArray[animationStackIndex]->Buffer());
			if(currentAnimationStack == NULL)
			{
				assert(false && "this is a problem. The animation stack should be found in the scene!");
				break;
			}

			FbxTime::EMode timeMode = scene->GetGlobalSettings().GetTimeMode();
			FbxTime frameTime;
			frameTime.SetTime(0, 0, 0, 1, 0, timeMode);

			if(samplingRate <= 0)
			{
				samplingRate = static_cast<unsigned int>(frameTime.GetFrameRate(timeMode));
			}
			float samplingTime = 1.0f / samplingRate;
			takeInfo.samplingRate = samplingTime;

			FbxTime samplingStep;
			samplingStep.SetTime(0, 0, 1, 0, 0, timeMode);
			samplingStep = static_cast<FbxLongLong>(samplingStep.Get() * samplingTime);

			FbxTakeInfo* currentTakeInfo = scene->GetTakeInfo(*(animationStackNamesArray[animationStackIndex]));
			if(currentTakeInfo)
			{
				FbxTime start = currentTakeInfo->mLocalTimeSpan.GetStart();
				FbxTime stop = currentTakeInfo->mLocalTimeSpan.GetStop();
				takeInfo.keyframeCount = static_cast<int>(((stop - start) / samplingStep).Get()) + 1;
			}
			else
			{
				FbxTimeSpan timeLineTimeSpan;
				scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeLineTimeSpan);

				FbxTime start = timeLineTimeSpan.GetStart();
				FbxTime stop = timeLineTimeSpan.GetStop();
				takeInfo.keyframeCount = static_cast<int>(((stop - start) / samplingStep).Get()) + 1;
			}
			animationTakes.push_back(takeInfo);
		}

		for (int i = 0; i < animationStacksCount; i++)
		{
			delete animationStackNamesArray[i];
		}
	}

	void Geometric::FetchMaterials(FbxMesh* fbxMesh, std::vector<Material>& materials)
	{
		// Fetch material properties
		const int materialsCount = fbxMesh->GetNode()->GetMaterialCount();
		if(materialsCount > 0)
		{
			materials.resize(materialsCount);
		}
		else
		{
			materials.resize(1);
		}

		for (int materialIndex = 0; materialIndex < materialsCount; ++materialIndex)
		{
			Material& material = materials.at(materialIndex);

			const FbxSurfaceMaterial* surfaceMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			material.name = surfaceMaterial->GetName();

			std::function<void(Material::Property&, std::string, std::string)> FetchMaterialProperty = [&](Material::Property& material, std::string propertyName, std::string factorName)
			{
				const FbxProperty property = surfaceMaterial->FindProperty(propertyName.c_str());
				const FbxProperty factor = surfaceMaterial->FindProperty(factorName.c_str());
				if (property.IsValid() && factor.IsValid())
				{
					FbxDouble3 color = property.Get<FbxDouble3>();
					material.color.x = static_cast<float>(color[0]);
					material.color.y = static_cast<float>(color[1]);
					material.color.z = static_cast<float>(color[2]);
					material.color.w = 1.0f;
				}

				if (property.IsValid())
				{
					const int texturesCount = property.GetSrcObjectCount<FbxFileTexture>();
					if (texturesCount > 0)
					{
						const FbxFileTexture* fileTexture = property.GetSrcObject < FbxFileTexture>();
						if (fileTexture)
						{
							material.textureFileName = fileTexture->GetRelativeFileName();
							material.uScale = static_cast<float>(fileTexture->GetScaleU());
							material.vScale = static_cast<float>(fileTexture->GetScaleV());

						}
					}
				}
			};
			FetchMaterialProperty(material.diffuse, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
			FetchMaterialProperty(material.ambient, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
			FetchMaterialProperty(material.specular, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
			FetchMaterialProperty(material.normalMap, FbxSurfaceMaterial::sNormalMap, FbxSurfaceMaterial::sBumpFactor);
			FetchMaterialProperty(material.bump, FbxSurfaceMaterial::sBump, FbxSurfaceMaterial::sBumpFactor);
		}

		if(materialsCount > 0)
		{
			const int polygonCount = fbxMesh->GetPolygonCount();
			for(int polygonIndex = 0;polygonIndex < polygonCount; ++polygonIndex)
			{
				const unsigned int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
				materials.at(materialIndex).indexCount += 3;
			}

			int offset = 0;
			for(Material& material : materials)
			{
				material.indexStart = offset;
				offset += material.indexCount;
				material.indexCount = 0;
			}
		}
	}

	Geometric::Vertex FetchVertex(FbxMesh* fbxMesh,FbxStringList& uvNames,int polygonIndex,int edgeIndex)
	{
		Geometric::Vertex vertex;

		const bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
		const bool hasTangent = fbxMesh->GetElementTangentCount() > 0;
		const bool hasUV = fbxMesh->GetElementUVCount() > 0;
		const bool hasVertexColor = fbxMesh->GetElementVertexColorCount() > 0;

		const FbxVector4* controlPointsArray = fbxMesh->GetControlPoints();
		const int controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, edgeIndex);

		vertex.position.x = static_cast<float>(controlPointsArray[controlPointIndex][0]);
		vertex.position.y = static_cast<float>(controlPointsArray[controlPointIndex][1]);
		vertex.position.z = static_cast<float>(controlPointsArray[controlPointIndex][2]);

		if(hasNormal)
		{
#if 1
			FbxVector4 normal;
			fbxMesh->GetPolygonVertexNormal(polygonIndex, edgeIndex, normal);
			vertex.normal.x = static_cast<float>(normal[0]);
			vertex.normal.y = static_cast<float>(normal[1]);
			vertex.normal.z = static_cast<float>(normal[2]);
#else
			
			const FbxGeometryElementNormal* geometry_element_normal = fbx_mesh->GetElementNormal(0);
			FbxGeometryElement::EMappingMode mapping_mode = geometry_element_normal->GetMappingMode();
			FbxGeometryElement::EReferenceMode reference_mode = geometry_element_normal->GetReferenceMode();

			if (mapping_mode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				int index_of_vertex = 0;
				switch (reference_mode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					index_of_vertex = index_of_control_point;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					index_of_vertex = geometry_element_normal->GetIndexArray().GetAt(index_of_control_point);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.normal.x = static_cast<float>(geometry_element_normal->GetDirectArray().GetAt(index_of_vertex)[0]);
				vertex.normal.y = static_cast<float>(geometry_element_normal->GetDirectArray().GetAt(index_of_vertex)[1]);
				vertex.normal.z = static_cast<float>(geometry_element_normal->GetDirectArray().GetAt(index_of_vertex)[2]);
			}
			else if (mapping_mode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				int index_of_vertex = 0;
				switch (reference_mode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					index_of_vertex = index_of_polygon * 3 + index_of_edge;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					index_of_vertex = geometry_element_normal->GetIndexArray().GetAt(index_of_control_point);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.normal.x = static_cast<float>(geometry_element_normal->GetDirectArray().GetAt(index_of_vertex)[0]);
				vertex.normal.y = static_cast<float>(geometry_element_normal->GetDirectArray().GetAt(index_of_vertex)[1]);
				vertex.normal.z = static_cast<float>(geometry_element_normal->GetDirectArray().GetAt(index_of_vertex)[2]);
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

			FbxGeometryElement::EMappingMode mappingMode = geometryElementTangent->GetMappingMode();

			FbxGeometryElement::EReferenceMode referenceMode = geometryElementTangent->GetReferenceMode();

			if(mappingMode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				int vertexIndex = 0;
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
				vertex.tanget.x = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.tanget.y = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.tanget.z = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else if(mappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				int vertexIndex = 0;
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
				vertex.tanget.x = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.tanget.y = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.tanget.z = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else
			{
				_ASSERT_EXPR(false, L"Invalid Mapping Mode");
				throw std::exception("Invalid Mapping Mode");
			}
		}

		if(hasUV)
		{
#if 1
			FbxVector2 uv;
			bool unmappedUV = false;
			fbxMesh->GetPolygonVertexUV(polygonIndex, edgeIndex, uvNames[0], uv, unmappedUV);
			vertex.texCoord.x = static_cast<float>(uv[0]);
			vertex.texCoord.y = static_cast<float>(uv[1]);
#else
			const FbxGeometryElementUV* geometry_element_uv = fbx_mesh->GetElementUV(0);
			FbxGeometryElement::EMappingMode mapping_mode = geometry_element_uv->GetMappingMode();
			FbxGeometryElement::EReferenceMode reference_mode = geometry_element_uv->GetReferenceMode();
			if (mapping_mode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				int index_of_vertex = 0;
				switch (reference_mode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					index_of_vertex = index_of_control_point;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					index_of_vertex = geometry_element_uv->GetIndexArray().GetAt(index_of_control_point);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.texcoord.x = static_cast<float>(geometry_element_uv->GetDirectArray().GetAt(index_of_vertex)[0]);
				vertex.texcoord.y = static_cast<float>(geometry_element_uv->GetDirectArray().GetAt(index_of_vertex)[1]);
			}
			else if (mapping_mode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				int index_of_vertex = 0;
				switch (reference_mode)
				{
				case FbxGeometryElement::EReferenceMode::eDirect:
					index_of_vertex = index_of_polygon * 3 + index_of_edge;
					break;
				case FbxGeometryElement::EReferenceMode::eIndexToDirect:
					index_of_vertex = geometry_element_uv->GetIndexArray().GetAt(index_of_control_point);
					break;
				case FbxGeometryElement::EReferenceMode::eIndex:
				default:
					_ASSERT_EXPR(false, L"Invalid Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.texcoord.x = static_cast<float>(geometry_element_uv->GetDirectArray().GetAt(index_of_vertex)[0]);
				vertex.texcoord.y = static_cast<float>(geometry_element_uv->GetDirectArray().GetAt(index_of_vertex)[1]);
			}
			else
			{
				_ASSERT_EXPR(false, L"Invalid Mapping Mode");
				throw std::exception("Invalid Mapping Mode");
			}
#endif
		}
		
		if(hasVertexColor)
		{
			const FbxGeometryElementVertexColor* geometryElementVertexColor = fbxMesh->GetElementVertexColor(0);
			FbxGeometryElement::EMappingMode mappingMode = geometryElementVertexColor->GetMappingMode();
			FbxGeometryElement::EReferenceMode referenceMode = geometryElementVertexColor->GetReferenceMode();

			if(mappingMode == FbxGeometryElement::EMappingMode::eByControlPoint)
			{
				int vertexIndex = 0;
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
					_ASSERT_EXPR(false, L"Invaild Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.color.x = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.color.y = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.color.z = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else if(mappingMode == FbxGeometryElement::EMappingMode::eByPolygonVertex)
			{
				int vertexIndex = 0;
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
					_ASSERT_EXPR(false, L"Invaild Reference Mode");
					throw std::exception("Invalid Reference Mode");
				}
				vertex.color.x = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[0]);
				vertex.color.y = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[1]);
				vertex.color.z = static_cast<float>(geometryElementVertexColor->GetDirectArray().GetAt(vertexIndex)[2]);
			}
			else
			{
				_ASSERT_EXPR(false, L"Invaild Mapping Mode");
				throw std::exception("Invalid Mapping Mode");
			}
		}
		return vertex;
	}

	Geometric::Vertex FetchVertex(FbxMesh* fbxMesh,FbxStringList& uvNames,int controlPointIndex)
	{
		Geometric::Vertex vertex;

		const bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
		const bool hasTangent = fbxMesh->GetElementTangentCount() > 0;
		const bool hasUV = fbxMesh->GetElementUVCount() > 0;
		const bool hasVertexColor = fbxMesh->GetElementVertexColorCount() > 0;

		const FbxVector4* controlPointsArray = fbxMesh->GetControlPoints();

		vertex.position.x = static_cast<float>(controlPointsArray[controlPointIndex][0]);
		vertex.position.y = static_cast<float>(controlPointsArray[controlPointIndex][1]);
		vertex.position.z = static_cast<float>(controlPointsArray[controlPointIndex][2]);

		if (hasNormal)
		{
			const FbxGeometryElementNormal* geometryElementNormal = fbxMesh->GetElementNormal(0);
			int vertexIndex = controlPointIndex;
			if(geometryElementNormal->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				vertexIndex = geometryElementNormal->GetIndexArray().GetAt(controlPointIndex);
			}
			vertex.normal.x = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[0]);
			vertex.normal.y = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[1]);
			vertex.normal.z = static_cast<float>(geometryElementNormal->GetDirectArray().GetAt(vertexIndex)[2]);
		}
		if(hasTangent)
		{
			const FbxGeometryElementTangent* geometryElementTangent = fbxMesh->GetElementTangent(0);
			int vertexIndex = controlPointIndex;
			if(geometryElementTangent->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				vertexIndex = geometryElementTangent->GetIndexArray().GetAt(controlPointIndex);
			}
			vertex.tanget.x = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[0]);
			vertex.tanget.y = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[1]);
			vertex.tanget.z = static_cast<float>(geometryElementTangent->GetDirectArray().GetAt(vertexIndex)[2]);
		}
		if(hasUV)
		{
			const FbxGeometryElementUV* geometryElementUV = fbxMesh->GetElementUV(0);
			int vertexIndex = controlPointIndex;
			if(geometryElementUV->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
			{
				vertexIndex = geometryElementUV->GetIndexArray().GetAt(controlPointIndex);
			}
			vertex.texCoord.x = static_cast<float>(geometryElementUV->GetDirectArray().GetAt(vertexIndex)[0]);
			vertex.texCoord.y = static_cast<float>(geometryElementUV->GetDirectArray().GetAt(vertexIndex)[1]);


			bool flippingVCoordinates = true;
			if (flippingVCoordinates)
			{
				vertex.texCoord.y = 1 - vertex.texCoord.y;
			}
		}
		if(hasVertexColor)
		{
			const FbxGeometryElementVertexColor* geometryElementVertexColor = fbxMesh->GetElementVertexColor(0);
			int vertexIndex = controlPointIndex;
			if(geometryElementVertexColor->GetReferenceMode() == FbxGeometryElement::EReferenceMode::eIndexToDirect)
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

	void Geometric::FetchVerticesAndIndices(FbxMesh* fbxMesh, std::vector<Vertex>& vertices, std::vector<unsigned>& indices, std::vector<Material>& materials)
	{
		bool allControlPoint = true;
		const bool hasNormal = fbxMesh->GetElementNormalCount() > 0;
		const bool hasTangent = fbxMesh->GetElementTangentCount() > 0;
		const bool hasUV = fbxMesh->GetElementUVCount() > 0;
		const bool hasVertexColor = fbxMesh->GetElementVertexColorCount() > 0;

		if(hasNormal && fbxMesh->GetElementNormal(0)->GetMappingMode() != FbxGeometryElement::eByControlPoint)
		{
			allControlPoint = false;
		}
		if(hasTangent && fbxMesh->GetElementTangent(0)->GetMappingMode() != FbxGeometryElement::eByControlPoint)
		{
			allControlPoint = false;
		}
		if (hasUV && fbxMesh->GetElementUV(0)->GetMappingMode() != FbxGeometryElement::eByControlPoint)
		{
			allControlPoint = false;
		}
		if (hasVertexColor && fbxMesh->GetElementVertexColor(0)->GetMappingMode() != FbxGeometryElement::eByControlPoint)
		{
			allControlPoint = false;
		}

		const int polygonCount = fbxMesh->GetPolygonCount();

		std::vector<BoneInfluencesPerControlPoint> boneInfluences;
		FetchBoneInfluence(fbxMesh, boneInfluences);

		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);


		// Fetch Vertices
		if (allControlPoint)
		{
			const int controlPointCount = fbxMesh->GetControlPointsCount();
			for (int controlPointIndex = 0; controlPointIndex < controlPointCount; ++controlPointIndex)
			{
				Vertex vertex = FetchVertex(fbxMesh, uvNames, controlPointIndex);

				BoneInfluencesPerControlPoint influencesPerControlPoint = boneInfluences.at(controlPointIndex);
				SetBoneInfluencesToVertex(vertex, influencesPerControlPoint);

				vertices.push_back(vertex);
			}
		}
		else
		{
			for (int polygonIndex = 0; polygonIndex < polygonCount; polygonIndex++)
			{
				for(int edgeIndex = 0;edgeIndex < 3;edgeIndex++)
				{
					Geometric::Vertex vertex = FetchVertex(fbxMesh, uvNames, polygonIndex, edgeIndex);

					const int controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, edgeIndex);
					BoneInfluencesPerControlPoint influencesPerControlPoint = boneInfluences.at(controlPointIndex);
					SetBoneInfluencesToVertex(vertex, influencesPerControlPoint);

					vertices.push_back(vertex);
				}
			}
		}

		// Re:build indices array
		unsigned int vertexCount = 0;
		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
		indices.resize(polygonCount * 3);

		for(int polygonIndex = 0;polygonIndex < polygonCount;polygonIndex++)
		{
			int materialIndex = 0;
			if(materialCount > 0)
			{
				materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
			}

			Material& material = materials.at(materialIndex);
			const int offsetIndex = material.indexStart + material.indexCount;

			for (int edgeIndex = 0; edgeIndex < 3; edgeIndex++)
			{
				const int controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, edgeIndex);
				if(allControlPoint)
				{
					indices.at(offsetIndex + edgeIndex) = static_cast<unsigned int>(controlPointIndex);
				}
				else
				{
					indices.at(offsetIndex + edgeIndex) = static_cast<unsigned int>(vertexCount);
				}
				vertexCount++;
			}
			material.indexCount += 3;
		}

		if(!hasNormal)
		{
			CalculateNormalVector(vertices, indices);
		}
		if(!hasTangent && hasUV && hasNormal)
		{
			CalculateTangentVector(vertices, indices);
		}
	}

	void CalculateTangentVector(std::vector<Geometric::Vertex>& vertices, const std::vector<unsigned>& indices)
	{
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			Geometric::Vertex* vertex[3] = { &vertices.at(indices.at(i + 0)),&vertices.at(indices.at(i + 1)),&vertices.at(indices.at(i + 2)) };

			float x1 = vertex[1]->position.x - vertex[0]->position.x;
			float y1 = vertex[1]->position.y - vertex[0]->position.y;
			float z1 = vertex[1]->position.z - vertex[0]->position.z;
			float x2 = vertex[2]->position.x - vertex[0]->position.x;
			float y2 = vertex[2]->position.y - vertex[0]->position.y;
			float z2 = vertex[2]->position.z - vertex[0]->position.z;

			float s1 = vertex[1]->texCoord.x - vertex[0]->texCoord.x;
			float t1 = vertex[1]->texCoord.y - vertex[0]->texCoord.y;

			float s2 = vertex[2]->texCoord.x - vertex[0]->texCoord.x;
			float t2 = vertex[2]->texCoord.y - vertex[0]->texCoord.y;

			float r = 1.0f / (s1 * t2 - s2 * t1);

			Vector3 T = Vector3((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			Vector3 B = Vector3((s2 * x1 - s1 * x2) * r, (s2 * y1 - s1 * y2) * r, (s2 * z1 - s1 * z2) * r);

			for (Geometric::Vertex* v : vertex)
			{
				Vector3 N = v->normal;
				v->tanget = Vector3(T - (N * T.Dot(N))).Normalize();

				if(N.Cross(T).Dot(B).x < 0)
				{
					v->tanget.x *= -1;
					v->tanget.y *= -1;
					v->tanget.z *= -1;
				}
			}
		}
	}

	void CalculateNormalVector(std::vector<Geometric::Vertex>& vertices, const std::vector<unsigned>& indices)
	{
		
	}


}

