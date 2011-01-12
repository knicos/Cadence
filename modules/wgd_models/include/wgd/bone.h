#ifndef _WGD_BONE_
#define _WGD_BONE_

#include <wgd/matrix.h>
#include <vector>

namespace wgd{
	class Bone {
		public:
		Bone(int id):  m_bid(id) {};
		~Bone() {};
		
		// Child bones //
		void addChild(const cadence::doste::OID &name) { m_child.push_back(name); };
		int children(){ return m_child.size(); };
		cadence::doste::OID& child(int i){ return m_child[i]; };
		
		// Skin Offset Matrix //
		void sMatrix(float *mat){ m_sMatrix = matrix(mat); };
		void sMatrix(const matrix &mat){ m_sMatrix = mat; };
		const matrix &sMatrix(){ return m_sMatrix; }
		
		// Base Transform Matrix //
		void tMatrix(float *mat){ m_tMatrix = matrix(mat); };
		void tMatrix(const matrix &mat){ m_tMatrix = matrix(mat); };
		const matrix &tMatrix(){ return m_tMatrix; }
		
		// Bone ID for linking with mesh //
		int bid() const { return m_bid; };
		
		private:
		
		//ID of this bone - used by IModel when transforming meshes
		int m_bid;
		
		//bone contains various matrices
		matrix m_sMatrix; // Skin Offset Matrix
		matrix m_tMatrix; // Base Transformation Matrix
		
		//child bones (by name)
		std::vector<cadence::doste::OID> m_child;
	};
};

#endif
