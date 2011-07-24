/* -------------------------------------------------------------------------
// WINX: a C++ template GUI library - MOST SIMPLE BUT EFFECTIVE
// 
// This file is a part of the WINX Library.
// The use and distribution terms for this software are covered by the
// Common Public License 1.0 (http://opensource.org/licenses/cpl.php)
// which can be found in the file CPL.txt at this distribution. By using
// this software in any fashion, you are agreeing to be bound by the terms
// of this license. You must not remove this notice, or any other, from
// this software.
// 
// Module: wrapper/pcre.h
// Creator: xushiwei
// Email: xushiweizh@gmail.com
// Date: 2009-7-2 23:57:09
// 
// $Id: pcre.h,v 2009-7-2 23:57:09 xushiwei Exp $
// -----------------------------------------------------------------------*/
#ifndef WRAPPER_PCRE_H
#define WRAPPER_PCRE_H

#ifndef WRAPPER_PCREAPI_H
#include "pcreapi.h"
#endif

#ifndef STDEXT_TEXT_BASICSTRING_H
#include "../../../stdext/include/stdext/text/BasicString.h"
#endif

#ifndef PCRE_CALL
#define PCRE_CALL	winx_call
#endif

#ifndef PCRE_SUBMATCH_MAX
#define PCRE_SUBMATCH_MAX	32
#endif

// -------------------------------------------------------------------------
// class PCRE

class PCRE
{
private:
	PCRE(const PCRE&);
	void operator=(const PCRE&);

public:
	struct Error
	{
		const char* message;
		int offset;
	};

	typedef NS_STDEXT::String String;

protected:
	pcre* m_re;
	Error m_error;

public:
	PCRE() : m_re(NULL) {
	}
	explicit PCRE(const char* pattern, int options = 0) {
		m_re = pcre_compile(pattern, options, &m_error.message, &m_error.offset, NULL);
	}
	~PCRE()
	{
		if (m_re)
			pcre_free(m_re);
	}

	operator const pcre*() const {
		return m_re;
	}

	void PCRE_CALL clear() {
		if (m_re) {
			pcre_free(m_re);
			m_re = NULL;
		}
	}

	bool PCRE_CALL init(const char* pattern, int options = 0) {
		WINX_ASSERT(m_re == NULL);
		m_re = pcre_compile(pattern, options, &m_error.message, &m_error.offset, NULL);
		return m_re != NULL;
	}

	bool PCRE_CALL good() const {
		return m_re != NULL;
	}

	const Error& PCRE_CALL error() const {
		return m_error;
	}

public:
	__forceinline int PCRE_CALL match(
		const String& subject, String submatches[], int max_count, int options = 0) const
	{
		return match(m_re, NULL, subject, submatches, max_count, options);
	}

	static int PCRE_CALL match(
		const pcre* re, const pcre_extra* extra,
		const String subject, String submatches[], int max_count, int options = 0)
	{
		WINX_ASSERT(re != NULL);
		WINX_ASSERT(sizeof(String) >= sizeof(int)*2);

		int* offsets = (int*)(submatches + max_count) - (max_count << 1);
		const int n = pcre_exec(
			re, extra, subject.data(), (int)subject.size(), 0,
			options, offsets, (max_count << 1));
		for (int i = 0; i < n; ++i)
		{
			submatches[i] = String(
				subject.begin() + offsets[(i << 1)],
				subject.begin() + offsets[(i << 1) + 1]);
		}
		return n;
	}

private:
	template <class Iterator>
	static int PCRE_CALL atoi_(
		Iterator first, Iterator last, Iterator& next, int val = 0)
	{
		for (; first != last && isdigit(*first); ++first)
			val = val * 10 + (*first - '0');
		next = first;
		return val;
	}

public:
	template <class StringBuilderT>
	__forceinline int PCRE_CALL replace(
		StringBuilderT& dest, const String& subject, const String& pattern,
		int options = 0, const char escch = '\\') const
	{
		return replace(m_re, NULL, dest, subject, pattern, options, escch);
	}

	template <class StringBuilderT>
	static int PCRE_CALL replace(
		const pcre* re, const pcre_extra* extra,
		StringBuilderT& dest, String subject, const String& pattern,
		int options = 0, const char escch = '\\')
	{
		String submatches[PCRE_SUBMATCH_MAX];
		int n = 0;
		for (;;)
		{
			const int count = match(
				re, extra, subject, submatches, countof(submatches), options);
			if (count < 0) {
				dest.insert(dest.end(), subject.begin(), subject.end());
				break;
			}
			dest.insert(dest.end(), subject.begin(), submatches[0].begin());
			const bool fOk = template_gen(dest, pattern, submatches, count, escch);
			if (!fOk) // failed!
				return -1;
			subject = String(submatches[0].end(), subject.end());
			++n;
		}
		return n;
	}

	template <class StringBuilderT>
	static bool PCRE_CALL template_gen(
		StringBuilderT& dest, const String& pattern,
		const String submatches[], int count, const char escch = '\\')
	{
		int i;
		String::const_iterator it, from = pattern.begin();
		String::const_iterator const to = pattern.end();
		for (;;)
		{
			it = std::find(from, to, escch);
			if (it == to)
				break;

			dest.insert(dest.end(), from, it);
			if (++it == to) {
				dest.push_back(escch);
				return true;
			}

			if (isdigit(*it)) {
				i = atoi_(it+1, to, from, *it - '0');
			}
			else if (*it == '{') {
				i = atoi_(it+1, to, from);
				if (from == to || *from != '}')
					return false;
				++from;
			}
			else if (*it == escch) {
				dest.push_back(escch);
				from = ++it;
				continue;
			}
			else {
				WINX_ASSERT(false && "PCRE::replace - unknown escape-character!");
				return false;
			}

			WINX_ASSERT(i >= 0 && i < count);
			if (i >= count)
				return false;

			const String sub = submatches[i];
			dest.insert(dest.end(), sub.begin(), sub.end());
		}
		dest.insert(dest.end(), from, to);
		return true;
	}
};

// -------------------------------------------------------------------------
// class FastPCRE

class FastPCRE : public PCRE
{
protected:
	pcre_extra* m_re_extra;

public:
	FastPCRE() : m_re_extra(NULL) {}
	explicit FastPCRE(const char* pattern, int options = 0) : PCRE(pattern, options) {
		m_re_extra = (m_re ? pcre_study(m_re, 0, &m_error.message) : NULL);
	}
	~FastPCRE() {
		if (m_re_extra)
			pcre_free(m_re_extra);
	}

	void PCRE_CALL clear()
	{
		if (m_re_extra) {
			pcre_free(m_re_extra);
			m_re_extra = NULL;
		}
		PCRE::clear();
	}

	bool PCRE_CALL init(const char* pattern, int options = 0)
	{
		if (PCRE::init(pattern, options)) {
			m_re_extra = (m_re ? pcre_study(m_re, 0, &m_error.message) : NULL);
			return true;
		}
		return false;
	}

	__forceinline int PCRE_CALL match(
		const String& subject, String submatches[], int max_count, int options = 0) const
	{
		return PCRE::match(m_re, m_re_extra, subject, submatches, max_count, options);
	}

	template <class StringBuilderT>
	__forceinline int PCRE_CALL replace(
		StringBuilderT& dest, const String& subject, const String& pattern,
		int options = 0, const char escch = '\\') const
	{
		return PCRE::replace(m_re, m_re_extra, dest, subject, pattern, options, escch);
	}
};

// -------------------------------------------------------------------------

#endif /* WRAPPER_PCRE_H */
