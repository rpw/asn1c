#define	__NO_ASN_TABLE__
#include <GeneralizedTime.c>
#include <constraints.c>

static void
check(char *time_str, time_t expect, int as_gmt) {
	GeneralizedTime_t gt;
	struct tm tm;
	time_t tloc;

	gt.buf = time_str;
	gt.size = strlen(time_str);

	tloc = asn_GT2time(&gt, &tm, as_gmt);
	printf("%s: [%s] -> %ld == %ld\n",
		as_gmt?"GMT":"ofs", time_str, (long)tloc, (long)expect);

	if(tloc != -1) {
		printf("\t%04d-%02d-%02dT%02d:%02d:%02d%+03ld%02ld\n",
		tm.tm_year + 1900,
		tm.tm_mon + 1,
		tm.tm_mday,
		tm.tm_hour,
		tm.tm_min,
		tm.tm_sec,
		(GMTOFF(tm) / 3600),
		labs(GMTOFF(tm) % 3600)
		);
	}
	assert(tloc == expect);

#ifdef	HAVE_TM_GMTOFF
	assert(tloc == -1 || as_gmt == 0 || GMTOFF(tm) == 0);
#endif

	if(!as_gmt) check(time_str, expect, 1);
}

static void
rcheck(time_t tloc, const char *expect, int force_gmt) {
	GeneralizedTime_t *gt;
	struct tm tm, *tmp;

	tmp = localtime_r(&tloc, &tm);
	assert(tmp);

	gt = asn_time2GT(0, &tm, force_gmt);
	if(gt) {
		assert(expect);
		printf("[%s] vs [%s] (%d)\n",
			gt->buf, expect, force_gmt);
		assert(gt->size == (int)strlen(gt->buf));
		assert(!strcmp(gt->buf, expect));
	} else {
		assert(!expect);
	}
}

int
main(int ac, char **av) {

	(void)av;

	check("200401250", -1, 0);
	check("2004012509300", -1, 0);
	check("20040125093000-", -1, 0);
	check("20040125093007-0", -1, 0);
	check("20040125093007-080", -1, 0);
	check("200401250930.01Z", -1, 0);

	/* These six are from X.690:11.7.5 */
	check("19920520240000Z", -1, 0);  /* midnight represented incorrectly */
	//check("19920622123421.0Z", -1, 0);  /* spurious trailing zeros */
	//check("19920722132100.30Z", -1, 0); /* spurious trailing zeros */
	check("19920521000000Z", 706406400, 0);
	check("19920622123421Z", 709216461, 0);
	check("19920722132100.3Z", 711811260, 0);

	check("20040125093007Z", 1075023007, 0);
	check("20040125093007+00", 1075023007, 0);
	check("20040125093007.01+0000", 1075023007, 0);
	check("20040125093007,1+0000", 1075023007, 0);
	check("20040125093007-0800", 1075051807, 0);

	rcheck(1075023007, "20040125093007Z", 1);

	if(ac > 1) {
		/* These will be valid only inside PST time zone */
		check("20040125093007", 1075051807, 0);
		check("200401250930", 1075051800, 0);
		check("20040125093000,01", 1075051800, 0);
		check("20040125093000,1234", 1075051800, 0);

		rcheck(1075023007, "20040125013007-0800", 0);
	}

	return 0;
}

/*
 * Dummy function.
 */

asn_enc_rval_t
OCTET_STRING_encode_der(asn_TYPE_descriptor_t *td, void *ptr, int tag_mode, ber_tlv_tag_t tag, asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t erval;

	(void)td;
	(void)ptr;
	(void)tag_mode;
	(void)tag;
	(void)cb;
	(void)app_key;

	return erval;
}

asn_enc_rval_t
OCTET_STRING_encode_xer_ascii(asn_TYPE_descriptor_t *td, void *ptr, int ilevel, enum xer_encoder_flags_e flags, asn_app_consume_bytes_f *cb, void *app_key) {
	asn_enc_rval_t erval;

	(void)td;
	(void)ptr;
	(void)ilevel;
	(void)flags;
	(void)cb;
	(void)app_key;

	return erval;
}
