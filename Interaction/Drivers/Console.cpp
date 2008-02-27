// Wiim API ©2006 Eric B.
// http://digitalretrograde.com/projects/wiim/

// May be used and modified freely as long as this message is left intact

#include "Console.h"

void odprintf(const char *format, ...)
{
	char	buf[4096], *p = buf;
	va_list	args;

	va_start(args, format);
	//p += _vsnprintf_s(p, sizeof buf - 1, sizeof buf - 1, format, args);
	va_end(args);

	while ( p > buf  &&  isspace(p[-1]) )
			*--p = '\0';

	*p++ = '\r';
	*p++ = '\n';
	*p   = '\0';

	wchar_t wbuf[4096];
	size_t out = 0;
//	mbstowcs_s(&out, wbuf, 4096, buf, 4096); 

//	OutputDebugString(wbuf);
}
