/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "Seader"
 * 	found in "applications_user/seader/seader.asn1"
 * 	`asn1c -D applications_user/seader/lib/asn1 -no-gen-example -pdu=all`
 */

#include "CardDetails.h"

asn_TYPE_member_t asn_MBR_CardDetails_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct CardDetails, protocol),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_Protocol,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"protocol"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct CardDetails, csn),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"csn"
		},
	{ ATF_POINTER, 2, offsetof(struct CardDetails, atqa),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"atqa"
		},
	{ ATF_POINTER, 1, offsetof(struct CardDetails, sak),
		(ASN_TAG_CLASS_CONTEXT | (3 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"sak"
		},
};
static const int asn_MAP_CardDetails_oms_1[] = { 2, 3 };
static const ber_tlv_tag_t asn_DEF_CardDetails_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_CardDetails_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* protocol */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* csn */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 }, /* atqa */
    { (ASN_TAG_CLASS_CONTEXT | (3 << 2)), 3, 0, 0 } /* sak */
};
asn_SEQUENCE_specifics_t asn_SPC_CardDetails_specs_1 = {
	sizeof(struct CardDetails),
	offsetof(struct CardDetails, _asn_ctx),
	asn_MAP_CardDetails_tag2el_1,
	4,	/* Count of tags in the map */
	asn_MAP_CardDetails_oms_1,	/* Optional members */
	2, 0,	/* Root/Additions */
	-1,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_CardDetails = {
	"CardDetails",
	"CardDetails",
	&asn_OP_SEQUENCE,
	asn_DEF_CardDetails_tags_1,
	sizeof(asn_DEF_CardDetails_tags_1)
		/sizeof(asn_DEF_CardDetails_tags_1[0]), /* 1 */
	asn_DEF_CardDetails_tags_1,	/* Same as above */
	sizeof(asn_DEF_CardDetails_tags_1)
		/sizeof(asn_DEF_CardDetails_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_CardDetails_1,
	4,	/* Elements count */
	&asn_SPC_CardDetails_specs_1	/* Additional specs */
};

