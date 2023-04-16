/*
 * Generated by asn1c-0.9.29 (http://lionet.info/asn1c)
 * From ASN.1 module "Seader"
 * 	found in "applications_user/seader/seader.asn1"
 * 	`asn1c -D applications_user/seader/lib/asn1 -no-gen-example -pdu=all`
 */

#include "NFCCommand.h"

static asn_oer_constraints_t asn_OER_type_NFCCommand_constr_1 CC_NOTUSED = {
	{ 0, 0 },
	-1};
asn_per_constraints_t asn_PER_type_NFCCommand_constr_1 CC_NOTUSED = {
	{ APC_CONSTRAINED,	 1,  1,  0,  1 }	/* (0..1) */,
	{ APC_UNCONSTRAINED,	-1, -1,  0,  0 },
	0, 0	/* No PER value map */
};
asn_TYPE_member_t asn_MBR_NFCCommand_1[] = {
	{ ATF_NOFLAGS, 0, offsetof(struct NFCCommand, choice.nfcSend),
		(ASN_TAG_CLASS_CONTEXT | (1 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NFCSend,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"nfcSend"
		},
	{ ATF_NOFLAGS, 0, offsetof(struct NFCCommand, choice.nfcOff),
		(ASN_TAG_CLASS_CONTEXT | (2 << 2)),
		-1,	/* IMPLICIT tag at current level */
		&asn_DEF_NULL,
		0,
		{ 0, 0, 0 },
		0, 0, /* No default value */
		"nfcOff"
		},
};
static const asn_TYPE_tag2member_t asn_MAP_NFCCommand_tag2el_1[] = {
    { (ASN_TAG_CLASS_CONTEXT | (1 << 2)), 0, 0, 0 }, /* nfcSend */
    { (ASN_TAG_CLASS_CONTEXT | (2 << 2)), 1, 0, 0 } /* nfcOff */
};
asn_CHOICE_specifics_t asn_SPC_NFCCommand_specs_1 = {
	sizeof(struct NFCCommand),
	offsetof(struct NFCCommand, _asn_ctx),
	offsetof(struct NFCCommand, present),
	sizeof(((struct NFCCommand *)0)->present),
	asn_MAP_NFCCommand_tag2el_1,
	2,	/* Count of tags in the map */
	0, 0,
	-1	/* Extensions start */
};
asn_TYPE_descriptor_t asn_DEF_NFCCommand = {
	"NFCCommand",
	"NFCCommand",
	&asn_OP_CHOICE,
	0,	/* No effective tags (pointer) */
	0,	/* No effective tags (count) */
	0,	/* No tags (pointer) */
	0,	/* No tags (count) */
	{ &asn_OER_type_NFCCommand_constr_1, &asn_PER_type_NFCCommand_constr_1, CHOICE_constraint },
	asn_MBR_NFCCommand_1,
	2,	/* Elements count */
	&asn_SPC_NFCCommand_specs_1	/* Additional specs */
};

