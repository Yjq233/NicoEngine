#include "XMesh.h"
#include "XStructures.h"


CXModel::CXModel()
{
   m_root = NULL;
   m_animControl = NULL;
   m_boneMatrices = NULL;
   m_maxBones = 0;
   m_currentContainer = NULL;
   m_center = D3DXVECTOR3(0, 0, 0);
   m_radius = 0;
   m_currentAni = 0;
   m_numAnimations = 0;
   m_currentTrack = 0;
   m_speed = 1.0f;
   m_currentTime = 0;
   m_transition = 0.25f;
   m_device = NULL;
}


bool CXModel::LoadXFile(char *file)
{
   if(!m_device) return false;

   CD3DAllocate alh;

   // Load X mesh from a file.
   if(FAILED(D3DXLoadMeshHierarchyFromX(file, D3DXMESH_MANAGED,
      m_device, &alh, NULL, &m_root, &m_animControl))) return false;

   // Record max number of animation sets in the X model.
   if(m_animControl)
      m_numAnimations = m_animControl->GetMaxNumAnimationSets();

   // Setup Bones.
   if(m_root)
      {
         SetupMatrices((stD3DFrameEx*)m_root, NULL);

         m_boneMatrices = new D3DXMATRIX[m_maxBones];
         ZeroMemory(m_boneMatrices, sizeof(D3DXMATRIX)*m_maxBones);

         D3DXFrameCalculateBoundingSphere(m_root, &m_center,
                                          &m_radius);
      }

   // Set initialize animation.
   SetAnimation(0);

   return true;
}


void CXModel::Shutdown()
{
   if(m_root)
      {
         CD3DAllocate alh;
         D3DXFrameDestroy(m_root, &alh);
         m_root = NULL;
      }

   if(m_animControl) m_animControl->Release();
   m_animControl = NULL;

   if(m_boneMatrices) delete[] m_boneMatrices;
   m_boneMatrices = NULL;
}


void CXModel::SetupMatrices(stD3DFrameEx *inFrame,
                            LPD3DXMATRIX parentMatirx)
{
   if(!m_device) return;

   stD3DContainerEx *containerEx = (stD3DContainerEx*)inFrame->pMeshContainer;

   if(containerEx)
      {
         if(!m_currentContainer)
            m_currentContainer = containerEx;

         if(containerEx->pSkinInfo && containerEx->MeshData.pMesh)
            {
               D3DVERTEXELEMENT9 Declaration[MAX_FVF_DECL_SIZE];

               if(FAILED(containerEx->MeshData.pMesh->GetDeclaration(Declaration)))
                  return;

               containerEx->MeshData.pMesh->CloneMesh(D3DXMESH_MANAGED,
                  Declaration, m_device, &containerEx->originalMesh);

               m_maxBones = max(m_maxBones, containerEx->pSkinInfo->GetNumBones());

               for(unsigned int i = 0; i < containerEx->pSkinInfo->GetNumBones(); i++)
                  {   
                     stD3DFrameEx *temp = (stD3DFrameEx*)D3DXFrameFind(m_root,
                        containerEx->pSkinInfo->GetBoneName(i));

                     containerEx->boneMatrices[i] = &temp->finalMatrix;
                  }

            }
      }

   if(inFrame->pFrameSibling)
      SetupMatrices((stD3DFrameEx*)inFrame->pFrameSibling, parentMatirx);

   if(inFrame->pFrameFirstChild)
      SetupMatrices((stD3DFrameEx*)inFrame->pFrameFirstChild, &inFrame->finalMatrix);
}


void CXModel::Update(float time, const D3DXMATRIX *matWorld)
{
   time /= m_speed;
   m_currentTime += time;

   if(m_animControl) m_animControl->AdvanceTime(time, NULL);

   UpdateMatrices(m_root, matWorld);

   stD3DContainerEx *pMesh = m_currentContainer;

   if(pMesh && pMesh->pSkinInfo)
      {
         unsigned int numBones = pMesh->pSkinInfo->GetNumBones();

         for(unsigned int i = 0; i < numBones; i++)
            D3DXMatrixMultiply(&m_boneMatrices[i], pMesh->pSkinInfo->GetBoneOffsetMatrix(i), pMesh->boneMatrices[i]);

         void *srcPtr;
         pMesh->MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&srcPtr);

         void *destPtr;
         pMesh->originalMesh->LockVertexBuffer(0, (void**)&destPtr);

         pMesh->pSkinInfo->UpdateSkinnedMesh(m_boneMatrices, NULL, srcPtr, destPtr);

         pMesh->originalMesh->UnlockVertexBuffer();
         pMesh->MeshData.pMesh->UnlockVertexBuffer();
      }
}


void CXModel::Render()
{
	if(m_root)
	   DrawFrame(m_root);
}


void CXModel::SetAnimation(unsigned int index)
{
   if(index >= m_numAnimations || index == m_currentAni)
      return;

   m_currentAni = index;

   LPD3DXANIMATIONSET set;
   m_animControl->GetAnimationSet(m_currentAni, &set);

   unsigned long nextTrack = (m_currentTrack == 0 ? 1 : 0);

   // Set next track.
   m_animControl->SetTrackAnimationSet(nextTrack, set);
   set->Release();	

   // Take way all tracks.
   m_animControl->UnkeyAllTrackEvents(m_currentTrack);
   m_animControl->UnkeyAllTrackEvents(nextTrack);

   // Key current track.
   m_animControl->KeyTrackEnable(m_currentTrack, FALSE, m_currentTime + m_transition);
   m_animControl->KeyTrackSpeed(m_currentTrack, 0.0f, m_currentTime, m_transition, D3DXTRANSITION_LINEAR);
   m_animControl->KeyTrackWeight(m_currentTrack, 0.0f, m_currentTime, m_transition, D3DXTRANSITION_LINEAR);

   // Key next track.
   m_animControl->SetTrackEnable(nextTrack, TRUE);
   m_animControl->KeyTrackSpeed(nextTrack, 1.0f, m_currentTime, m_transition, D3DXTRANSITION_LINEAR);
   m_animControl->KeyTrackWeight(nextTrack, 1.0f, m_currentTime, m_transition, D3DXTRANSITION_LINEAR);

   m_currentTrack = nextTrack;
}


void CXModel::NextAnimation()
{	
   unsigned int newAnimationSet = m_currentAni + 1;

   if(newAnimationSet >= m_numAnimations)
      newAnimationSet = 0;

   SetAnimation(newAnimationSet);
}


void CXModel::UpdateMatrices(const D3DXFRAME *inFrame, const D3DXMATRIX *parentMatrix)
{
   stD3DFrameEx *frame = (stD3DFrameEx*)inFrame;

   if(parentMatrix != NULL)
      D3DXMatrixMultiply(&frame->finalMatrix, &frame->TransformationMatrix, parentMatrix);
   else
      frame->finalMatrix = frame->TransformationMatrix;

   if(frame->pFrameSibling != NULL)
      UpdateMatrices(frame->pFrameSibling, parentMatrix);

   if(frame->pFrameFirstChild != NULL)
      UpdateMatrices(frame->pFrameFirstChild, &frame->finalMatrix);
}


void CXModel::DrawFrame(LPD3DXFRAME frame)
{
   if(!m_device) return;

   LPD3DXMESHCONTAINER meshContainer = frame->pMeshContainer;

   while(meshContainer)
      {
         stD3DFrameEx *frameEx = (stD3DFrameEx*)frame;
		 m_device->SetTransform(D3DTS_WORLD, &frameEx->finalMatrix);

         stD3DContainerEx *containerEx = (stD3DContainerEx*)meshContainer;

         for(unsigned int i = 0; i < containerEx->NumMaterials; i++)
            {
               m_device->SetMaterial(&containerEx->mat[i]);
               m_device->SetTexture(0, containerEx->textures[i]);

               LPD3DXMESH pDrawMesh;
               
               if(containerEx->pSkinInfo)
                  pDrawMesh = containerEx->originalMesh;
               else
                  pDrawMesh = containerEx->MeshData.pMesh;

               pDrawMesh->DrawSubset(i);
            }

         meshContainer = meshContainer->pNextMeshContainer;
      }

   if(frame->pFrameSibling != NULL)
      DrawFrame(frame->pFrameSibling);

   if(frame->pFrameFirstChild != NULL)
      DrawFrame(frame->pFrameFirstChild);
}