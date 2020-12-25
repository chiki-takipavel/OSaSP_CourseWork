#include <Windows.h>

#define MAX_COUNT_FORMATS 17

TCHAR tAudioFormat[17][7] =
{
	TEXT(".3gp"),
	TEXT(".aac"),
	TEXT(".aax"),
	TEXT(".act"),
	TEXT(".aiff"),
	TEXT(".amr"),
	TEXT(".ape"),
	TEXT(".flac"),
	TEXT(".m4a"),
	TEXT(".mp3"),
	TEXT(".mpc"),
	TEXT(".ogg"),
	TEXT(".oga"),
	TEXT(".raw"),
	TEXT(".vox"),
	TEXT(".wav"),
	TEXT(".wma"),
};

TCHAR tVideoFormat[11][7] =
{
	TEXT(".webm"),
	TEXT(".mkv"),
	TEXT(".flv"),
	TEXT(".vob"),
	TEXT(".avi"),
	TEXT(".wmv"),
	TEXT(".rmvb"),
	TEXT(".mp4"),
	TEXT(".m4p"),
	TEXT(".mpg"),
	TEXT(".mpeg")
};

TCHAR tImageFormat[6][6] =
{
	TEXT(".jpg"),
	TEXT(".jpeg"),
	TEXT(".exif"),
	TEXT(".tiff"),
	TEXT(".gif"),
	TEXT(".png"),
};

TCHAR tDocumentFormat[6][6] =
{
	TEXT(".pdf"),
	TEXT(".txt"),
	TEXT(".doc"),
	TEXT(".docx"),
	TEXT(".pptx"),
	TEXT(".pub"),
};