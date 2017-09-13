#pragma once
#include "VertexLoader.h"

class MD5Animation {
	public:
		MD5Animation() : m_iMD5Version(0)
			, m_iNumFrames(0)
			, m_iNumJoints(0)
			, m_iFramRate(0)
			, m_iNumAnimatedComponents(0)
			, m_fAnimDuration(0)
			, m_fFrameDuration(0)
			, m_fAnimTime(0) {

		}

		virtual ~MD5Animation() {

		}

		// The JointInfo stores the information necessary to build the
		// skeletons for each frame
		struct JointInfo
		{
			std::string m_Name;
			int         m_ParentID;
			int         m_Flags;
			int         m_StartIndex;
		};
		typedef std::vector<JointInfo> JointInfoList;

		struct Bound
		{
			glm::vec3   m_Min;
			glm::vec3   m_Max;
		};
		typedef std::vector<Bound> BoundList;

		struct BaseFrame
		{
			glm::vec3   m_Pos;
			glm::quat   m_Orient;
		};
		typedef std::vector<BaseFrame> BaseFrameList;

		struct FrameData
		{
			int m_iFrameID;
			std::vector<float> m_FrameData;
		};
		typedef std::vector<FrameData> FrameDataList;

		// A Skeleton joint is a joint of the skeleton per frame
		struct SkeletonJoint
		{
			SkeletonJoint()
				: m_Parent(-1)
				, m_Pos(0)
			{}

			SkeletonJoint(const BaseFrame& copy)
				: m_Pos(copy.m_Pos)
				, m_Orient(copy.m_Orient)
			{}

			int         m_Parent;
			glm::vec3   m_Pos;
			glm::quat   m_Orient;
		};
		typedef std::vector<SkeletonJoint> SkeletonJointList;

		// A frame skeleton stores the joints of the skeleton for a single frame.
		struct FrameSkeleton
		{
			SkeletonJointList   m_Joints;
		};
		typedef std::vector<FrameSkeleton> FrameSkeletonList;

		const FrameSkeleton& GetSkeleton() const
		{
			return m_AnimatedSkeleton;
		}

		int GetNumJoints() const
		{
			return m_iNumJoints;
		}

		const JointInfo& GetJointInfo(unsigned int index) const
		{
			assert(index < m_JointInfos.size());
			return m_JointInfos[index];
		}

	private:
		void RemoveQuotes(std::string& str) {
			size_t n;
			while ((n = str.find('\"')) != std::string::npos) str.erase(n, 1);
		}

		void ComputeQuatW(glm::quat& quat) {
			float t = 1.0f - (quat.x * quat.x) - (quat.y * quat.y) - (quat.z * quat.z);
			if (t < 0.0f)
			{
				quat.w = 0.0f;
			}
			else
			{
				quat.w = -sqrtf(t);
			}
		}

		void IgnoreLine(std::istream& file, int length) {
			file.ignore(length, '\n');
		}

		std::streampos fileSize(const char* filePath) {

			std::streampos fsize = 0;
			std::ifstream file(filePath, std::ios::binary);

			fsize = file.tellg();
			file.seekg(0, std::ios::end);
			fsize = file.tellg() - fsize;
			file.close();

			return fsize;
		}

	public:
		bool LoadAnimation(const std::string& path, const std::string& animation) {
			std::string filename = "resources/models/" + path + "/" + animation + ".md5anim";

			if (!std::ifstream(filename)) {
				std::cerr << "MD5Model::LoadModel: Failed to find file: " << filename << std::endl;
				return false;
			}

			std::ifstream file(filename);
			std::string param("");
			std::string junk;   // Read junk from the file

			int fileLength = (int)fileSize(filename.c_str());
			assert(fileLength > 0);

			m_JointInfos.clear();
			m_Bounds.clear();
			m_BaseFrames.clear();
			m_Frames.clear();
			m_AnimatedSkeleton.m_Joints.clear();
			m_iNumFrames = 0;

			file >> param;

			while (!file.eof())
			{
				if (param == "MD5Version")
				{
					file >> m_iMD5Version;
					assert(m_iMD5Version == 10);
				}
				else if (param == "commandline")
				{
					file.ignore(fileLength, '\n'); // Ignore everything else on the line
				}
				else if (param == "numFrames")
				{
					file >> m_iNumFrames;
					file.ignore(fileLength, '\n');
				}
				else if (param == "numJoints")
				{
					file >> m_iNumJoints;
					file.ignore(fileLength, '\n');
				}
				else if (param == "frameRate")
				{
					file >> m_iFramRate;
					file.ignore(fileLength, '\n');
				}
				else if (param == "numAnimatedComponents")
				{
					file >> m_iNumAnimatedComponents;
					file.ignore(fileLength, '\n');
				}
				else if (param == "hierarchy")
				{
					file >> junk; // read in the '{' character
					for (int i = 0; i < m_iNumJoints; ++i)
					{
						JointInfo joint;
						file >> joint.m_Name >> joint.m_ParentID >> joint.m_Flags >> joint.m_StartIndex;
						RemoveQuotes(joint.m_Name);

						m_JointInfos.push_back(joint);

						file.ignore(fileLength, '\n');
					}
					file >> junk; // read in the '}' character
				}
				else if (param == "bounds")
				{
					file >> junk; // read in the '{' character
					file.ignore(fileLength, '\n');
					for (int i = 0; i < m_iNumFrames; ++i)
					{
						Bound bound;
						file >> junk; // read in the '(' character
						file >> bound.m_Min.x >> bound.m_Min.y >> bound.m_Min.z;
						file >> junk >> junk; // read in the ')' and '(' characters.
						file >> bound.m_Max.x >> bound.m_Max.y >> bound.m_Max.z;

						m_Bounds.push_back(bound);

						file.ignore(fileLength, '\n');
					}

					file >> junk; // read in the '}' character
					file.ignore(fileLength, '\n');
				}
				else if (param == "baseframe")
				{
					file >> junk; // read in the '{' character
					file.ignore(fileLength, '\n');

					for (int i = 0; i < m_iNumJoints; ++i)
					{
						BaseFrame baseFrame;
						file >> junk;
						file >> baseFrame.m_Pos.x >> baseFrame.m_Pos.y >> baseFrame.m_Pos.z;
						file >> junk >> junk;
						file >> baseFrame.m_Orient.x >> baseFrame.m_Orient.y >> baseFrame.m_Orient.z;
						file.ignore(fileLength, '\n');

						m_BaseFrames.push_back(baseFrame);
					}
					file >> junk; // read in the '}' character
					file.ignore(fileLength, '\n');
				}
				else if (param == "frame")
				{
					FrameData frame;
					file >> frame.m_iFrameID >> junk; // Read in the '{' character
					file.ignore(fileLength, '\n');

					for (int i = 0; i < m_iNumAnimatedComponents; ++i)
					{
						float frameData;
						file >> frameData;
						frame.m_FrameData.push_back(frameData);
					}

					m_Frames.push_back(frame);

					// Build a skeleton for this frame
					BuildFrameSkeleton(m_Skeletons, m_JointInfos, m_BaseFrames, frame);

					file >> junk; // Read in the '}' character
					file.ignore(fileLength, '\n');
				}

				file >> param;
			} // while ( !file.eof )

			  // Make sure there are enough joints for the animated skeleton.
			m_AnimatedSkeleton.m_Joints.assign(m_iNumJoints, SkeletonJoint());

			m_fFrameDuration = 1.0f / (float)m_iFramRate;
			m_fAnimDuration = (m_fFrameDuration * (float)m_iNumFrames);
			m_fAnimTime = 0.0f;

			assert(m_JointInfos.size() == m_iNumJoints);
			assert(m_Bounds.size() == m_iNumFrames);
			assert(m_BaseFrames.size() == m_iNumJoints);
			assert(m_Frames.size() == m_iNumFrames);
			assert(m_Skeletons.size() == m_iNumFrames);

			return true;
		}

		void Update(float fDeltaTime)
		{
			if (m_iNumFrames < 1) return;

			m_fAnimTime += fDeltaTime;

			while (m_fAnimTime > m_fAnimDuration) m_fAnimTime -= m_fAnimDuration;
			while (m_fAnimTime < 0.0f) m_fAnimTime += m_fAnimDuration;

			// Figure out which frame we're on
			float fFramNum = m_fAnimTime * (float)m_iFramRate;
			int iFrame0 = (int)floorf(fFramNum);
			int iFrame1 = (int)ceilf(fFramNum);
			iFrame0 = iFrame0 % m_iNumFrames;
			iFrame1 = iFrame1 % m_iNumFrames;

			float fInterpolate = fmodf(m_fAnimTime, m_fFrameDuration) / m_fFrameDuration;
			InterpolateSkeletons(m_AnimatedSkeleton, m_Skeletons[iFrame0], m_Skeletons[iFrame1], fInterpolate);
		}

	protected:

		JointInfoList       m_JointInfos;
		BoundList           m_Bounds;
		BaseFrameList       m_BaseFrames;
		FrameDataList       m_Frames;
		FrameSkeletonList   m_Skeletons;    // All the skeletons for all the frames

		FrameSkeleton       m_AnimatedSkeleton;

		// Build the frame skeleton for a particular frame
		void BuildFrameSkeleton(FrameSkeletonList& skeletons, const JointInfoList& jointInfos, const BaseFrameList& baseFrames, const FrameData& frameData)
		{
			FrameSkeleton skeleton;

			for (unsigned int i = 0; i < jointInfos.size(); ++i)
			{
				unsigned int j = 0;

				const JointInfo& jointInfo = jointInfos[i];
				// Start with the base frame position and orientation.
				SkeletonJoint animatedJoint = baseFrames[i];

				animatedJoint.m_Parent = jointInfo.m_ParentID;

				if (jointInfo.m_Flags & 1) // Pos.x
				{
					animatedJoint.m_Pos.x = frameData.m_FrameData[jointInfo.m_StartIndex + j++];
				}
				if (jointInfo.m_Flags & 2) // Pos.y
				{
					animatedJoint.m_Pos.y = frameData.m_FrameData[jointInfo.m_StartIndex + j++];
				}
				if (jointInfo.m_Flags & 4) // Pos.x
				{
					animatedJoint.m_Pos.z = frameData.m_FrameData[jointInfo.m_StartIndex + j++];
				}
				if (jointInfo.m_Flags & 8) // Orient.x
				{
					animatedJoint.m_Orient.x = frameData.m_FrameData[jointInfo.m_StartIndex + j++];
				}
				if (jointInfo.m_Flags & 16) // Orient.y
				{
					animatedJoint.m_Orient.y = frameData.m_FrameData[jointInfo.m_StartIndex + j++];
				}
				if (jointInfo.m_Flags & 32) // Orient.z
				{
					animatedJoint.m_Orient.z = frameData.m_FrameData[jointInfo.m_StartIndex + j++];
				}

				ComputeQuatW(animatedJoint.m_Orient);

				if (animatedJoint.m_Parent >= 0) // Has a parent joint
				{
					SkeletonJoint& parentJoint = skeleton.m_Joints[animatedJoint.m_Parent];
					glm::vec3 rotPos = parentJoint.m_Orient * animatedJoint.m_Pos;

					animatedJoint.m_Pos = parentJoint.m_Pos + rotPos;
					animatedJoint.m_Orient = parentJoint.m_Orient * animatedJoint.m_Orient;

					animatedJoint.m_Orient = glm::normalize(animatedJoint.m_Orient);
				}

				skeleton.m_Joints.push_back(animatedJoint);
			}

			skeletons.push_back(skeleton);
		}
		glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t) {
			return (1.0f - t)*a + t*b;
		}

		void InterpolateSkeletons(FrameSkeleton& finalSkeleton, const FrameSkeleton& skeleton0, const FrameSkeleton& skeleton1, float fInterpolate)
		{
			for (int i = 0; i < m_iNumJoints; ++i)
			{
				SkeletonJoint& finalJoint = finalSkeleton.m_Joints[i];
				const SkeletonJoint& joint0 = skeleton0.m_Joints[i];
				const SkeletonJoint& joint1 = skeleton1.m_Joints[i];

				finalJoint.m_Parent = joint0.m_Parent;

				finalJoint.m_Pos.x = joint0.m_Pos.x + fInterpolate * (joint1.m_Pos.x - joint0.m_Pos.x);
				finalJoint.m_Pos.y = joint0.m_Pos.y + fInterpolate * (joint1.m_Pos.y - joint0.m_Pos.y);
				finalJoint.m_Pos.z = joint0.m_Pos.z + fInterpolate * (joint1.m_Pos.z - joint0.m_Pos.z);

				finalJoint.m_Orient = glm::slerp(joint0.m_Orient, joint1.m_Orient, fInterpolate);
			}
		}

	private:
		int m_iMD5Version;
		int m_iNumFrames;
		int m_iNumJoints;
		int m_iFramRate;
		int m_iNumAnimatedComponents;

		float m_fAnimDuration;
		float m_fFrameDuration;
		float m_fAnimTime;
};