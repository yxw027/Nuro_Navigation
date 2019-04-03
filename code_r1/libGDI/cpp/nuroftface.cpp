// nuroftface.cpp: implementation of the nuroftface class.
//
//////////////////////////////////////////////////////////////////////

#include "nuroftface.h"

FT_CALLBACK_DEF(nuULONG) nuro_FT_Stream_IO( FT_Stream stream, nuULONG offset, unsigned char* buffer, nuULONG count )
{
	nuFILE* file = (nuFILE*)stream->descriptor.pointer;	
    if ( stream->pos != offset )
	{
		//fseek( file, offset, SEEK_SET );
		nuFseek(file, offset, NURO_SEEK_SET);
	}
	if (NULL == buffer)
	{
		return nuFALSE;
	}
    return (nuULONG)nuFread( buffer, 1, count, file );
}
FT_CALLBACK_DEF(nuVOID) nuro_FT_Stream_Close( FT_Stream  stream )
{
	nuFclose((nuFILE*)stream->descriptor.pointer);
    stream->descriptor.pointer = NULL;
    stream->size               = 0;
    stream->base               = 0;
}
FT_BASE_DEF(FT_Error) nuro_FT_Stream_Open(FT_Stream stream, nuFILE* fp)
{
    if ( !stream || !fp)
		return 0xffff;	
    stream->size = nuFgetlen(fp);
    stream->descriptor.pointer = fp;
    stream->pos                = 0;
    stream->read  = nuro_FT_Stream_IO;
    stream->close = nuro_FT_Stream_Close;
    return FT_Err_Ok;
}
FT_BASE_DEF(FT_Error) nuro_FT_Open_Face( FT_Library library, FT_Long face_index, FT_Face *aface, nuFILE* fp)
{
	FT_Stream    stream;
	FT_Error   error;
    FT_Memory  memory;
	if ( !library )
		return 0xffff;
    memory   = library->memory;
	if ( FT_NEW( stream ) )
		return 0xffff;
	if (nuro_FT_Stream_Open(stream, fp))
	{
		return 0xffff;
	}
	FT_Open_Args  args = {0};
	args.flags = FT_OPEN_STREAM;
	args.stream = stream;
    if (FT_Open_Face(library, &args, face_index, aface))
    {
        FT_Stream_Free(stream, 1);
        return 0xffff;
    }
	return 0;;
}
