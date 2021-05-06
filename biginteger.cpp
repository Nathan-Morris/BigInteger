#include "biginteger.h"

void BigInteger::fromIntegral(unsigned long long ull) {
	this->bytes.clear();
	unsigned char* bytePtr = (unsigned char*)&ull;
	for (unsigned int i = 0; i < 8; i++)
		this->bytes.push_back(flipBits(bytePtr[i]));
}

void BigInteger::fromIntegral(long long ll) {
	if ((this->isNegative = ll < 0))
		ll *= -1;
	this->fromIntegral((unsigned long long)ll);
}

BigInteger::BigInteger() { }

BigInteger::BigInteger(Bytes b) : bytes(b) { }

BigInteger::BigInteger(Bytes b, bool isNeg) 
	: isNegative(isNeg), bytes(b) { }

BigInteger::BigInteger(long long l) {
	this->fromIntegral(l);
}

BigInteger::BigInteger(unsigned long long l) {
	this->fromIntegral(l);
}

BigInteger::BigInteger(const BigInteger& ref) 
	: isNegative(ref.isNegative), bytes(ref.bytes) { }

BigInteger::~BigInteger() { }

std::string BigInteger::toStringBits() {
	std::stringstream ss;
	for (unsigned char c : this->bytes)
		ss << std::bitset<8>(c) << ' ';
	return ss.str();
}

void BigInteger::add(const BigInteger x, bool ignoreSign) {
	if (ignoreSign)
		goto op;

	if (x.isNegative != this->isNegative) {
		this->sub(x, true);
		return;
	}

op:
	BigInteger y = x, carry;
	while (!y.isZero()) {
		carry = *this & y;
		*this ^= y;
		y = carry >> 1;
	}
}

void BigInteger::sub(const BigInteger x, bool ignoreSign) {
	if (ignoreSign)
		goto op;

	if (x.isNegative != this->isNegative) {
		this->add(x, true);
		return;
	}

op:
	BigInteger y = x, carry;
	while (!y.isZero()) {
		carry = ~(*this) & y;
		*this ^= y;
		y = carry >> 1;
	}
}

void BigInteger::mult(BigInteger b) {
	BigInteger b2 = *this;
	while (1) {
		--b;
		if (b.isZero())
			return;
		this->add(b2, 1);
	}
}

void BigInteger::div(BigInteger b) {
	// TODO
}

bool BigInteger::lesserThan(const BigInteger& b, bool orEqual) const {
	if (this->isNegative && !b.isNegative)
		return 1;
	else if (b.isNegative && !this->isNegative)
		return 0;

	bool thisLarger = (this->bytes.size() > b.bytes.size() ? 1 : 0);
	const Bytes* larger = (thisLarger ? &this->bytes : &b.bytes);
	unsigned int minSz = MIN(this->bytes.size(), b.bytes.size());

	for (unsigned int i = larger->size() - 1; i >= minSz; i--)
		if (larger->at(i) != 0)
			return !thisLarger;

	for (unsigned int i = minSz - 1; i != UINT_MAX; i--)
		if (this->bytes.at(i) != b.bytes.at(i))
			return (b.bytes.at(i) > this->bytes.at(i));

	return orEqual;
}

bool BigInteger::greaterThan(const BigInteger& b, bool orEqual) const {
	if (b.isNegative && !this->isNegative)
		return 1;
	else if (this->isNegative && !b.isNegative)
		return 0;

	bool thisLarger = (this->bytes.size() > b.bytes.size() ? 1 : 0);
	const Bytes* larger = (thisLarger ? &this->bytes : &b.bytes);
	unsigned int minSz = MIN(this->bytes.size(), b.bytes.size());

	for (unsigned int i = larger->size() - 1; i >= minSz; i--)
		if (larger->at(i) != 0)
			return thisLarger;

	for (unsigned int i = minSz - 1; i != UINT_MAX; i--)
		if (this->bytes.at(i) != b.bytes.at(i))
			return (b.bytes.at(i) < this->bytes.at(i));

	return orEqual;
}

BigInteger BigInteger::getInvertedSign() const {
	return BigInteger(this->bytes, !this->isNegative);
}

bool BigInteger::isZero() const {
	for (unsigned char b : this->bytes)
		if (b != 0)
			return 0;
	return 1;
}

BigInteger& BigInteger::operator=(long long ll) {
	this->fromIntegral(ll);
	return *this;
}

BigInteger& BigInteger::operator=(unsigned long long ull) {
	this->fromIntegral(ull);
	return *this;
}

bool BigInteger::operator<(const BigInteger& b) {
	return this->lesserThan(b, false);
}

bool BigInteger::operator>(const BigInteger& b) {
	return this->greaterThan(b, false);
}

bool BigInteger::operator<=(const BigInteger& b) {
	return this->lesserThan(b, true);
}

bool BigInteger::operator>=(const BigInteger& b) {
	return this->greaterThan(b, true);
}

bool BigInteger::operator==(const BigInteger& b) {
	if (this->isZero() && b.isZero())
		return 1;
	if (this->isNegative != b.isNegative)
		return 0;
	
	unsigned int minSz = MIN(this->bytes.size(), b.bytes.size());

	for (unsigned int i = 0; i < minSz; i++)
		if (this->bytes[i] != b.bytes[i])
			return 0;

	const Bytes* larger = (this->bytes.size() > b.bytes.size() ? &this->bytes : &b.bytes);

	for (unsigned int i = minSz; i < larger->size(); i++) {
		if (larger->at(i) != 0)
			return 0;
	}

	return 1;
}

BigInteger BigInteger::operator~() {
	BigInteger b = *this;
	byteOpNot(b.bytes);
	return b;
}

bool BigInteger::operator!=(const BigInteger& b) {
	return !this->operator==(b);
}

BigInteger& BigInteger::operator&=(const BigInteger& b) {
	byteOpAnd(this->bytes, b.bytes);
	return *this;
}

BigInteger& BigInteger::operator|=(const BigInteger& b) {
	byteOpOr(this->bytes, b.bytes);
	return *this;
}

BigInteger& BigInteger::operator^=(const BigInteger& b) {
	byteOpXor(this->bytes, b.bytes);
	return *this;
}

BigInteger& BigInteger::operator--() {
	this->sub(BigInteger(1ll));
	return *this;
}

BigInteger& BigInteger::operator++() {
	this->add(BigInteger(1ll));
	return *this;
}

BigInteger operator&(const BigInteger& a, const BigInteger& b) {
	Bytes aCpy = a.bytes;
	byteOpAnd(aCpy, b.bytes);
	return BigInteger(aCpy);
}

BigInteger operator|(const BigInteger& a, const BigInteger& b) {
	Bytes aCpy = a.bytes;
	byteOpOr(aCpy, b.bytes);
	return BigInteger(aCpy);
}

BigInteger operator^(const BigInteger& a, const BigInteger& b) {
	Bytes aCpy = a.bytes;
	byteOpXor(aCpy, b.bytes);
	return BigInteger(aCpy);
}

BigInteger operator<<(const BigInteger& a, const unsigned int& shift) {
	BigInteger b = a;
	byteOpLeftShift(b.bytes, shift);
	return b;
}

BigInteger operator>>(const BigInteger& a, const unsigned int& shift) {
	BigInteger b = a;
	byteOpRightShift(b.bytes, shift, true);
	return b;
}
