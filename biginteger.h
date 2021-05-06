#include <iostream>
#include <sstream>
#include <bitset>
#include <vector>
#pragma once

#define MIN(_v1, _v2) (_v1 < _v2 ? _v1 : _v2)
#define MAX(_v1, _v2) (_v1 > _v2 ? _v1 : _v2)

#define BYTE_OP_XOR	1
#define BYTE_OP_OR	2
#define BYTE_OP_AND	3

typedef std::vector<unsigned char> Bytes;

static inline unsigned char flipBits(unsigned char b) {
	unsigned char c = 0;
	for (unsigned int i = 0; i < 8; i++)
		c |= ((b >> i) & 1) << (7 - i);
	return c;
}

static void bytesOps(
	Bytes& b1, // stores here
	const Bytes& b2,
	unsigned char op
) {
	Bytes bytes = (b1.size() > b2.size() ? b1 : b2);
	unsigned int minSz = MIN(b1.size(), b2.size());
	for (unsigned int i = 0; i != minSz; i++) {
		switch (op)
		{
		case BYTE_OP_XOR:
			b1[i] ^= b2[i];
			break;
		case BYTE_OP_OR:
			b1[i] |= b2[i];
			break;
		case BYTE_OP_AND:
			b1[i] &= b2[i];
			break;
		}
	}
}

#define byteOpXor(_b1, _b2)	bytesOps(_b1, _b2, BYTE_OP_XOR)
#define byteOpOr(_b1, _b2)	bytesOps(_b1, _b2, BYTE_OP_OR)
#define byteOpAnd(_b1, _b2)	bytesOps(_b1, _b2, BYTE_OP_AND)

static void byteOpNot(
	Bytes& b
) {
	for (unsigned int i = 0; i != b.size(); i++)
		b[i] = ~b[i];
}

static void byteOpLeftShift(
	Bytes& b,
	unsigned long shift
) {
	if (shift > 7) {
		unsigned long byteShift = shift / 8;

		for (unsigned long i = byteShift; i != b.size(); i++) {
			b[i - byteShift] = b[i];
		}

		for (unsigned long i = b.size() - byteShift; i != b.size(); i++) {
			b[i] = 0x0;
		}

		// faster '%= 8'
		shift &= 7;
	}

	if (!shift)
		return;

	unsigned long bSz = b.size();
	unsigned long bMaxOff = bSz - 1;
	unsigned char next = 0;
	for (unsigned long i = 0; i != bSz; i++) {
		if (i != bMaxOff)
			next = b[i + 1];
		else
			next = 0;

		// shift byte bits to left 'shift' amount
		b[i] <<= shift; 

		// shift next byte's bits to right by '8 - shift' amount
		next >>= 8 - shift;

		// performs 'OR' operation on bytes, effectively combines
		// 'b[i]' and 'next' on a bit level, not value
		b[i] |= next;
	}
}

static void byteOpRightShift(
	Bytes& b,
	unsigned long shift,
	bool makeNew = 0
) {
	if (shift > 7) {
		unsigned long byteShift = shift / 8;

		if (makeNew) {
			b.insert(b.begin(), byteShift, 0);
		}
		else {
			for (unsigned long i = b.size() - byteShift - 1; i != ULONG_MAX; i--) {
				b[i + byteShift] = b[i];
			}

			for (unsigned long i = 0; i != byteShift; i++) {
				b[i] = 0x0;
			}
		}

		// faster '%= 8'
		shift &= 7;
	}

	if (!shift)
		return;

	unsigned long bSz = b.size();
	unsigned long bMaxOff = bSz - 1;
	unsigned char prev = 0;
	for (unsigned long i = bMaxOff; i != ULONG_MAX; i--) {
		if (i)
			prev = b[i - 1];
		else
			prev = 0;

		// shift byte bits to right 'shift' amount
		b[i] >>= shift;

		// shift next byte's bits to left by '8 - shift' amount
		prev <<= 8 - shift;

		// performs 'OR' operation on bytes, effectively combines
		// 'b[i]' and 'next' on a bit level, not value
		b[i] |= prev;
	}
}

class BigInteger
{
private:
	bool isNegative = 0;
	Bytes bytes;

	void fromIntegral(long long);
	void fromIntegral(unsigned long long);

public:
	BigInteger();

	BigInteger(Bytes);
	BigInteger(Bytes, bool);

	BigInteger(unsigned long long);
	BigInteger(long long);

	BigInteger(const BigInteger&);

	~BigInteger();

	template<typename _INTEGRAL = unsigned long long>
	_INTEGRAL toIntegral(unsigned long startOffset = 0) {
		if (startOffset + sizeof(_INTEGRAL) > this->bytes.size())
			this->bytes.resize(startOffset + sizeof(_INTEGRAL), 0);
		Bytes retV(sizeof(_INTEGRAL));
		for (unsigned int i = 0; i < sizeof(_INTEGRAL); i++)
			retV[i] = flipBits(this->bytes[i + startOffset]);
		return *(_INTEGRAL*)retV.data();
	}

	std::string toStringBits();

	inline void addByte(unsigned char c = 0) {
		this->bytes.push_back(c);
	}

	void add(const BigInteger, bool ignoreSign = false);
	void sub(const BigInteger, bool ignoreSign = false);

	void mult(BigInteger);
	void div(BigInteger);

	bool lesserThan(const BigInteger&, bool) const;
	bool greaterThan(const BigInteger&, bool) const;

	BigInteger getInvertedSign() const;

	bool isZero() const;

	BigInteger& operator=(long long);
	BigInteger& operator=(unsigned long long);

	bool operator<(const BigInteger&);
	bool operator>(const BigInteger&);
	
	bool operator<=(const BigInteger&);
	bool operator>=(const BigInteger&);

	bool operator==(const BigInteger&);
	bool operator!=(const BigInteger&);

	BigInteger& operator&=(const BigInteger&);
	BigInteger& operator|=(const BigInteger&);
	BigInteger& operator^=(const BigInteger&);

	BigInteger& operator--();
	BigInteger& operator++();

	BigInteger operator~();

	friend BigInteger operator&(const BigInteger&, const BigInteger&);
	friend BigInteger operator|(const BigInteger&, const BigInteger&);
	friend BigInteger operator^(const BigInteger&, const BigInteger&);

	friend BigInteger operator<<(const BigInteger&, const unsigned int&);
	friend BigInteger operator>>(const BigInteger&, const unsigned int&);
};