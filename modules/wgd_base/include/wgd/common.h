#ifndef _WGD_COMMON_H_
#define _WGD_COMMON_H_

#include <wgd/index.h>
#include <wgd/dll.h>
#include <cadence/messages.h>

#if !defined(PACKED)
#ifndef _MSC_VER
#define PACKED __attribute__ ((packed))
#else
#define PACKED
#endif
#endif

#if !defined(ALIGNED)
#ifndef _MSC_VER
#define ALIGNED __attribute__ ((aligned(16)))
#define ALIGNVS
#else
#define ALIGNED
#define ALIGNVS __declspec(align(16)) 
#endif
#endif



#include <cadence/doste/oid.h>
namespace wgd {
	typedef cadence::doste::OID OID;
	static const OID Null = cadence::doste::Null;
	
	extern BASEIMPORT OID WGD;
};

#endif
