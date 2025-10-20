//---------------------------------------------------------------------------

#ifndef FileControlH
#define FileControlH
//---------------------------------------------------------------------------
class FileControl
{
private:
	static FileControl* instance;
	FileControl();
public:
	static FileControl& getInstance();
	void HideStart(void);
	void ExecOHTProgram(void);
	void CloseOHTProgram(void);
	void CloseExceptProgram(void);

	void ExecFMProgram(void);
	void CloseFMProgram(void);

	void ExecProgram(String);
	void CloseProgram(String);
	void CloseExceptProgram(String);
	void Delete_File(String);
	void Create_Directory(String);
	void Delete_Directory(String);
};

FileControl* FileControl::instance = NULL;

#endif
