/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "Seader"
 * 	found in "seader.asn1"
 * 	`asn1c -D ./lib/asn1 -no-gen-example -pdu=all`
 */

#include "SamVersion.h"

static asn_TYPE_member_t asn_MBR_SamVersion_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct SamVersion, version),
		(ASN_TAG_CLASS_CONTEXT | (0 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"version"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SamVersion, firmware),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"firmware"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct SamVersion, type),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_OCTET_STRING,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"type"
		},
};
static const ber_tlv_tag_t asn_DEF_SamVersion_tags_1[] = {
	(ASN_TAG_CLASS_UNIVERSAL | (16 << 2))
};
static const asn_TYPE_tag2member_t asn_MAP_SamVersion_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (0 << 2)), 0, 0, 0 }, /* version */
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 1, 0, 0 }, /* firmware */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 2, 0, 0 } /* type */
};
static asn_SEQUENCE_specifics_t asn_SPC_SamVersion_specs_1 = {
	sizeof(struct SamVersion),
	offsetof(struct SamVersion, _asn_ctx),
	asn_MAP_SamVersion_tag2el_1,
	3,	/* Count of tags in the map */
	0, 0, 0,	/* Optional elements (not needed) */
	-1,	/* First extension addition */
};
asn_TYPE_descriptor_t asn_DEF_SamVersion = {
	"SamVersion",
	"SamVersion",
	&asn_OP_SEQUENCE,
	asn_DEF_SamVersion_tags_1,
	sizeof(asn_DEF_SamVersion_tags_1)
		/sizeof(asn_DEF_SamVersion_tags_1[0]), /* 1 */
	asn_DEF_SamVersion_tags_1,	/* Same as above */
	sizeof(asn_DEF_SamVersion_tags_1)
		/sizeof(asn_DEF_SamVersion_tags_1[0]), /* 1 */
	{ 0, 0, SEQUENCE_constraint },
	asn_MBR_SamVersion_1,
	3,	/* Elements count */
	&asn_SPC_SamVersion_specs_1	/* Additional specs */
};

