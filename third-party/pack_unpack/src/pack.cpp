#include "pack.h"

unsigned long long int pack754(long double f, unsigned int bits, unsigned int expbits) {
  long double fnorm;
  size_t shift;
  long long sign, exp, significand;
  unsigned significandbits = bits - expbits - 1; // -1 for sign bit

  if (f == 0.0) return 0; // get this special case out of the way

  // check sign and begin normalization
  if (f < 0) { sign = 1; fnorm = -f; }
  else { sign = 0; fnorm = f; }

  // get the normalized form of f and track the exponent
  shift = 0;
  while(fnorm >= 2.0) { fnorm /= 2.0; shift++; }
  while(fnorm < 1.0) { fnorm *= 2.0; shift--; }
  fnorm = fnorm - 1.0;

  // calculate the binary form (non-float) of the significand data
  significand = fnorm * ((1LL<<significandbits) + 0.5f);

  // get the biased exponent
  exp = shift + ((1<<(expbits-1)) - 1); // shift + bias

  // return the final answer
  return (sign<<(bits-1)) | (exp<<(bits-expbits-1)) | significand;
}

long double unpack754(uint64_t i, unsigned int bits, unsigned int expbits) {
  long double result;
  size_t shift;
  size_t bias;
  unsigned significandbits = bits - expbits - 1; // -1 for sign bit

  if (i == 0) return 0.0;

  // pull the significand
  result = (i&((1LL<<significandbits)-1)); // mask
  result /= (1LL<<significandbits); // convert back to float
  result += 1.0f; // add the one back on

  // deal with the exponent
  bias = (1<<(expbits-1)) - 1;
  shift = ((i>>significandbits)&((1LL<<expbits)-1)) - bias;
  while(shift > 0) { result *= 2.0; shift--; }
  while(shift < 0) { result /= 2.0; shift++; }

  // sign it
  result *= (i>>(bits-1))&1? -1.0: 1.0;

  return result;
}

void packi16(unsigned char *buf, unsigned int i) {
  *buf++ = i>>8; *buf++ = i;
}

void packi32(unsigned char *buf, unsigned long i) {
  *buf++ = i>>24; *buf++ = i>>16;
  *buf++ = i>>8;  *buf++ = i;
}

void packi64(unsigned char *buf, unsigned long long int i) {
  *buf++ = i>>56; *buf++ = i>>48;
  *buf++ = i>>40; *buf++ = i>>32;
  *buf++ = i>>24; *buf++ = i>>16;
  *buf++ = i>>8;  *buf++ = i;
}

int unpacki16(const unsigned char *buf) {
  uint16_t i2 = ((uint16_t)buf[0]<<8) | buf[1];
  int16_t i;

  // change unsigned numbers to signed
  if (i2 <= 0x7fffu) { i = i2; }
  else { i = -1 - (int16_t)(0xffffu - i2); }

  return i;
}

uint16_t unpacku16(unsigned char *buf) {
  return ((uint16_t)buf[0]<<8) | buf[1];
}

int32_t unpacki32(const unsigned char *buf) {
  uint32_t i2 = ((uint32_t)buf[0]<<24) |
	  ((uint32_t)buf[1]<<16) |
	  ((uint32_t)buf[2]<<8)  |
	  buf[3];
  int32_t i;

  // change unsigned numbers to signed
  if (i2 <= 0x7fffffffu) { i = i2; }
  else { i = -1 -(int32_t)(0xffffffffu - i2); }

  return i;
}

uint32_t unpacku32(const unsigned char *buf) {
  return ((uint32_t)buf[0]<<24) |
	  ((uint32_t)buf[1]<<16) |
	  ((uint32_t)buf[2]<<8)  |
	  buf[3];
}

int64_t unpacki64(const unsigned char *buf) {
  uint64_t i2 = ((uint64_t)buf[0]<<56) |
	  ((uint64_t)buf[1]<<48) |
	  ((uint64_t)buf[2]<<40) |
	  ((uint64_t)buf[3]<<32) |
	  ((uint64_t)buf[4]<<24) |
	  ((uint64_t)buf[5]<<16) |
	  ((uint64_t)buf[6]<<8)  |
	  buf[7];
  int64_t i;

  // change unsigned numbers to signed
  if (i2 <= 0x7fffffffffffffffu) { i = i2; }
  else { i = -1 -(int64_t)(0xffffffffffffffffu - i2); }

  return i;
}

uint64_t unpacku64(const unsigned char *buf) {
  return ((uint64_t)buf[0]<<56) |
	  ((uint64_t)buf[1]<<48) |
	  ((uint64_t)buf[2]<<40) |
	  ((uint64_t)buf[3]<<32) |
	  ((uint64_t)buf[4]<<24) |
	  ((uint64_t)buf[5]<<16) |
	  ((uint64_t)buf[6]<<8)  |
	  buf[7];
}

unsigned int pack(unsigned char *buf, char *format, ...) {
  va_list ap;

  int8_t c;
  uint8_t C;

  int16_t h;
  uint16_t H;

  int32_t l;
  uint32_t L;

  int64_t q;
  uint64_t Q;

  float f;                    // floats
  double d;
  long double g;
  uint64_t fhold;

  char *s;                    // strings
  size_t len;

  size_t size = 0;

  va_start(ap, format);

  for(; *format != '\0'; format++) {
	switch(*format) {
	  case 'c': // 8-bit
		size += 1;
		c = (int8_t)va_arg(ap, int); // promoted
		*buf++ = c;
		break;

	  case 'C': // 8-bit unsigned
		size += 1;
		C = (uint8_t)va_arg(ap, unsigned int); // promoted
		*buf++ = C;
		break;

	  case 'h': // 16-bit
		size += 2;
		h = va_arg(ap, int);
		packi16(buf, h);
		buf += 2;
		break;

	  case 'H': // 16-bit unsigned
		size += 2;
		H = va_arg(ap, unsigned int);
		packi16(buf, H);
		buf += 2;
		break;

	  case 'l': // 32-bit
		size += 4;
		l = va_arg(ap, long int);
		packi32(buf, l);
		buf += 4;
		break;

	  case 'L': // 32-bit unsigned
		size += 4;
		L = va_arg(ap, unsigned long int);
		packi32(buf, L);
		buf += 4;
		break;

	  case 'q': // 64-bit
		size += 8;
		q = va_arg(ap, long long int);
		packi64(buf, q);
		buf += 8;
		break;

	  case 'Q': // 64-bit unsigned
		size += 8;
		Q = va_arg(ap, unsigned long long int);
		packi64(buf, Q);
		buf += 8;
		break;

	  case 'f': // float-16
		size += 2;
		f = (float)va_arg(ap, double); // promoted
		fhold = pack754_16(f); // convert to IEEE 754
		packi16(buf, fhold);
		buf += 2;
		break;

	  case 'd': // float-32
		size += 4;
		d = va_arg(ap, double);
		fhold = pack754_32(d); // convert to IEEE 754
		packi32(buf, fhold);
		buf += 4;
		break;

	  case 'g': // float-64
		size += 8;
		g = va_arg(ap, long double);
		fhold = pack754_64(g); // convert to IEEE 754
		packi64(buf, fhold);
		buf += 8;
		break;

	  case 's': // string
		s = va_arg(ap, char*);
		len = strlen(s);
		size += len + 2;
		packi16(buf, len);
		buf += 2;
		memcpy(buf, s, len);
		buf += len;
		break;
	}
  }

  va_end(ap);

  return size;
}

void unpack(unsigned char *buf, char *format, ...) {
  va_list ap;

  int8_t *c;              // 8-bit
  uint8_t *C;

  int16_t *h;                      // 16-bit
  uint16_t *H;

  int32_t *l;                 // 32-bit
  uint32_t *L;

  int64_t *q;            // 64-bit
  uint64_t *Q;

  float *f;                    // floats
  double *d;
  long double *g;
  unsigned long long int fhold;

  char *s;
  size_t len, maxstrlen=0, count;

  va_start(ap, format);

  for(; *format != '\0'; format++) {
	switch(*format) {
	  case 'c': // 8-bit
		c = va_arg(ap, signed char*);
		if (*buf <= 0x7f) { *c = *buf;} // re-sign
		else { *c = -1 - (unsigned char)(0xffu - *buf); }
		buf++;
		break;

	  case 'C': // 8-bit unsigned
		C = va_arg(ap, unsigned char*);
		*C = *buf++;
		break;

	  case 'h': // 16-bit
		h = va_arg(ap, int16_t*);
		*h = unpacki16(buf);
		buf += 2;
		break;

	  case 'H': // 16-bit unsigned
		H = va_arg(ap, uint16_t*);
		*H = unpacku16(buf);
		buf += 2;
		break;

	  case 'l': // 32-bit
		l = va_arg(ap, int32_t*);
		*l = unpacki32(buf);
		buf += 4;
		break;

	  case 'L': // 32-bit unsigned
		L = va_arg(ap, uint32_t*);
		*L = unpacku32(buf);
		buf += 4;
		break;

	  case 'q': // 64-bit
		q = va_arg(ap, int64_t*);
		*q = unpacki64(buf);
		buf += 8;
		break;

	  case 'Q': // 64-bit unsigned
		Q = va_arg(ap, uint64_t*);
		*Q = unpacku64(buf);
		buf += 8;
		break;

	  case 'f': // float
		f = va_arg(ap, float*);
		fhold = unpacku16(buf);
		*f = unpack754_16(fhold);
		buf += 2;
		break;

	  case 'd': // float-32
		d = va_arg(ap, double*);
		fhold = unpacku32(buf);
		*d = unpack754_32(fhold);
		buf += 4;
		break;

	  case 'g': // float-64
		g = va_arg(ap, long double*);
		fhold = unpacku64(buf);
		*g = unpack754_64(fhold);
		buf += 8;
		break;

	  case 's': // string
		s = va_arg(ap, char*);
		len = unpacku16(buf);
		buf += 2;
		if (maxstrlen > 0 && len > maxstrlen) count = maxstrlen - 1;
		else count = len;
		memcpy(s, buf, count);
		s[count] = '\0';
		buf += len;
		break;

	  default:
		if (isdigit(*format)) { // track max str len
		  maxstrlen = maxstrlen * 10 + (*format-'0');
		}
	}

	if (!isdigit(*format)) maxstrlen = 0;
  }

  va_end(ap);
}
