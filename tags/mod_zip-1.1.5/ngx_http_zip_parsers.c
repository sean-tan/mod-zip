#line 1 "ngx_http_zip_parsers.rl"
/* Parser definitions for mod_zip */

#include "ngx_http_zip_module.h"
#include "ngx_http_zip_parsers.h"

static void
ngx_http_zip_file_init(ngx_http_zip_file_t *parsing_file)
{
    parsing_file->uri.data = NULL;
    parsing_file->uri.len = 0;

    parsing_file->args.data = NULL;
    parsing_file->args.len = 0;

    parsing_file->filename.data = NULL;
    parsing_file->filename.len = 0;

    parsing_file->header_sent = 0;
    parsing_file->trailer_sent = 0;

    parsing_file->crc32 = 0;
    parsing_file->size = 0;
}

static ngx_int_t
ngx_http_zip_clean_range(ngx_http_zip_range_t *range,
        int prefix, int suffix, ngx_http_zip_ctx_t *ctx)
{
    if (suffix) {
        range->end = ctx->archive_size;
        range->start = ctx->archive_size - range->start;
    } else if (prefix) {
        range->end = ctx->archive_size;
    } else {
        range->end++;
        /*
         * Download Accelerator sends the last byte position
         * that equals to the file length
         */
        if (range->end >= ctx->archive_size) {
            range->end = ctx->archive_size;
        }
    }
    if (range->start < 0) {
        return NGX_ERROR;
    }
    if (range->start >= ctx->archive_size) {
        return NGX_ERROR;
    }
    return NGX_OK;
}


#line 56 "ngx_http_zip_parsers.c"
static const char _request_actions[] = {
	0, 1, 1, 1, 2, 1, 3, 1, 
	4, 1, 5, 1, 6, 1, 7, 1, 
	8, 2, 0, 6
};

static const char _request_key_offsets[] = {
	0, 0, 7, 8, 11, 14, 16, 18, 
	19, 22, 25, 32, 33, 34
};

static const char _request_trans_keys[] = {
	45, 48, 57, 65, 70, 97, 102, 32, 
	32, 48, 57, 32, 48, 57, 32, 63, 
	32, 63, 32, 0, 10, 13, 0, 10, 
	13, 32, 48, 57, 65, 70, 97, 102, 
	32, 32, 10, 13, 45, 48, 57, 65, 
	70, 97, 102, 0
};

static const char _request_single_lengths[] = {
	0, 1, 1, 1, 1, 2, 2, 1, 
	3, 3, 1, 1, 1, 3
};

static const char _request_range_lengths[] = {
	0, 3, 0, 1, 1, 0, 0, 0, 
	0, 0, 3, 0, 0, 3
};

static const char _request_index_offsets[] = {
	0, 0, 5, 7, 10, 13, 16, 19, 
	21, 25, 29, 34, 36, 38
};

static const char _request_indicies[] = {
	0, 2, 2, 2, 1, 3, 1, 3, 
	4, 1, 5, 4, 1, 5, 1, 6, 
	8, 9, 7, 11, 10, 1, 1, 1, 
	12, 1, 13, 13, 12, 3, 14, 14, 
	14, 1, 1, 15, 17, 16, 18, 18, 
	0, 2, 2, 2, 1, 0
};

static const char _request_trans_targs[] = {
	2, 0, 10, 3, 4, 5, 6, 6, 
	7, 11, 8, 7, 9, 13, 10, 12, 
	12, 7, 13
};

static const char _request_trans_actions[] = {
	17, 0, 17, 0, 9, 0, 1, 0, 
	3, 3, 13, 0, 0, 15, 11, 5, 
	0, 7, 0
};

static const int request_start = 1;
static const int request_first_final = 13;
static const int request_error = 0;

static const int request_en_main = 1;

#line 56 "ngx_http_zip_parsers.rl"


ngx_int_t 
ngx_http_zip_parse_request(ngx_http_zip_ctx_t *ctx)
{
    int cs;
    u_char *p = ctx->unparsed_request->data;
    u_char *pe = ctx->unparsed_request->data + ctx->unparsed_request->len;
    ngx_http_zip_file_t *parsing_file = NULL;

    
#line 131 "ngx_http_zip_parsers.c"
	{
	cs = request_start;
	}

#line 136 "ngx_http_zip_parsers.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _request_trans_keys + _request_key_offsets[cs];
	_trans = _request_index_offsets[cs];

	_klen = _request_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _request_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _request_indicies[_trans];
	cs = _request_trans_targs[_trans];

	if ( _request_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _request_actions + _request_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 68 "ngx_http_zip_parsers.rl"
	{
            parsing_file = ngx_array_push(&ctx->files);
            ngx_http_zip_file_init(parsing_file);

            parsing_file->index = ctx->files.nelts - 1;
        }
	break;
	case 1:
#line 75 "ngx_http_zip_parsers.rl"
	{
            parsing_file->uri.data = p;
            parsing_file->uri.len = 1;
        }
	break;
	case 2:
#line 80 "ngx_http_zip_parsers.rl"
	{
            parsing_file->uri.len = p - parsing_file->uri.data;
        }
	break;
	case 3:
#line 83 "ngx_http_zip_parsers.rl"
	{
            parsing_file->args.data = p;
        }
	break;
	case 4:
#line 86 "ngx_http_zip_parsers.rl"
	{
            parsing_file->args.len = p - parsing_file->args.data;
        }
	break;
	case 5:
#line 89 "ngx_http_zip_parsers.rl"
	{
            parsing_file->size = parsing_file->size * 10 + ((*p) - '0');
        }
	break;
	case 6:
#line 92 "ngx_http_zip_parsers.rl"
	{
            if ((*p) == '-') {
                ctx->missing_crc32 = 1;
            } else {
                parsing_file->crc32 *= 16;
                if ((*p) >= 'a' && (*p) <= 'f') {
                    parsing_file->crc32 += (*p) - 'a' + 10;
                }
                else if ((*p) >= 'A' && (*p) <= 'F') {
                    parsing_file->crc32 += (*p) - 'A' + 10;
                } else { /* 0-9 */
                    parsing_file->crc32 += (*p) - '0';
                }
            }
        }
	break;
	case 7:
#line 107 "ngx_http_zip_parsers.rl"
	{
            parsing_file->filename.data = p;
        }
	break;
	case 8:
#line 110 "ngx_http_zip_parsers.rl"
	{
            parsing_file->filename.len = p - parsing_file->filename.data;
        }
	break;
#line 280 "ngx_http_zip_parsers.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}
#line 129 "ngx_http_zip_parsers.rl"


    if (cs < request_first_final) {
        return NGX_ERROR;
    }

    ctx->parsed = 1;

    return NGX_OK;
}


#line 305 "ngx_http_zip_parsers.c"
static const char _range_actions[] = {
	0, 1, 0, 1, 1, 1, 2, 2, 
	0, 1, 2, 3, 1
};

static const char _range_key_offsets[] = {
	0, 0, 1, 2, 3, 4, 5, 6, 
	9, 11, 14, 17
};

static const char _range_trans_keys[] = {
	98, 121, 116, 101, 115, 61, 45, 48, 
	57, 48, 57, 45, 48, 57, 44, 48, 
	57, 44, 48, 57, 0
};

static const char _range_single_lengths[] = {
	0, 1, 1, 1, 1, 1, 1, 1, 
	0, 1, 1, 1
};

static const char _range_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 1, 
	1, 1, 1, 1
};

static const char _range_index_offsets[] = {
	0, 0, 2, 4, 6, 8, 10, 12, 
	15, 17, 20, 23
};

static const char _range_trans_targs[] = {
	2, 0, 3, 0, 4, 0, 5, 0, 
	6, 0, 7, 0, 8, 9, 0, 10, 
	0, 11, 9, 0, 7, 10, 0, 7, 
	11, 0, 0
};

static const char _range_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 1, 7, 0, 10, 
	0, 0, 3, 0, 0, 3, 0, 0, 
	5, 0, 0
};

static const int range_start = 1;
static const int range_first_final = 10;
static const int range_error = 0;

static const int range_en_main = 1;

#line 143 "ngx_http_zip_parsers.rl"


ngx_int_t
ngx_http_zip_parse_range(ngx_http_request_t *r, ngx_str_t *range_str, ngx_http_zip_ctx_t *ctx)
{
    int cs, prefix = 0, suffix = 0;

    ngx_http_zip_range_t *range = NULL;
    u_char *p = range_str->data;
    u_char *pe = range_str->data + range_str->len;

    
#line 370 "ngx_http_zip_parsers.c"
	{
	cs = range_start;
	}

#line 375 "ngx_http_zip_parsers.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _range_trans_keys + _range_key_offsets[cs];
	_trans = _range_index_offsets[cs];

	_klen = _range_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _range_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	cs = _range_trans_targs[_trans];

	if ( _range_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _range_actions + _range_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 155 "ngx_http_zip_parsers.rl"
	{
            if (range) {
                if (ngx_http_zip_clean_range(range, prefix, suffix, ctx) == NGX_ERROR) {
                    return NGX_ERROR;
                }
            }
            if ((range = ngx_array_push(&ctx->ranges)) == NULL) {
                return NGX_ERROR;
            }
            range->start = 0; range->end = 0; range->boundary_sent = 0;
            suffix = 0;
            prefix = 1;
        }
	break;
	case 1:
#line 169 "ngx_http_zip_parsers.rl"
	{ range->start = range->start * 10 + ((*p) - '0'); }
	break;
	case 2:
#line 171 "ngx_http_zip_parsers.rl"
	{ range->end = range->end * 10 + ((*p) - '0'); prefix = 0; }
	break;
	case 3:
#line 173 "ngx_http_zip_parsers.rl"
	{ suffix = 1; }
	break;
#line 476 "ngx_http_zip_parsers.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}
#line 186 "ngx_http_zip_parsers.rl"


    if (cs < range_first_final) {
        return NGX_ERROR;
    }

    if (range) {
        if (ngx_http_zip_clean_range(range, prefix, suffix, ctx) == NGX_ERROR) {
            return NGX_ERROR;
        }
    }

    return NGX_OK;
}
