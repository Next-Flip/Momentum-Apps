/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "Seader"
 * 	found in "applications_user/seader/seader.asn1"
 * 	`asn1c -D applications_user/seader/lib/asn1 -no-gen-example -pdu=all`
 */

#ifndef	_ErrorResponse_H_
#define	_ErrorResponse_H_


#include <asn_application.h>

/* Including external dependencies */
#include <NativeInteger.h>
#include <OCTET_STRING.h>
#include <constr_SEQUENCE.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ErrorResponse */
typedef struct ErrorResponse {
	long	 errorCode;
	OCTET_STRING_t	 data;
	
	/* Context for parsing across buffer boundaries */
	asn_struct_ctx_t _asn_ctx;
} ErrorResponse_t;

/* Implementation */
extern asn_TYPE_descriptor_t asn_DEF_ErrorResponse;
extern asn_SEQUENCE_specifics_t asn_SPC_ErrorResponse_specs_1;
extern asn_TYPE_member_t asn_MBR_ErrorResponse_1[2];

#ifdef __cplusplus
}
#endif

#endif	/* _ErrorResponse_H_ */
#include <asn_internal.h>
