//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <Vcl.Dialogs.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iomanip>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TButton *GetParametrsButton;
	TButton *EncodeButton;
	TButton *DecodeButton;
	TEdit *QEdit;
	TEdit *PEdit;
	TLabel *Label1;
	TLabel *Label2;
	TEdit *DEdit;
	TEdit *EEdit;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *REdit;
	TEdit *FEdit;
	TLabel *Label5;
	TLabel *Label6;
	TMemo *InputMemo;
	TMemo *OutputMemo;
	TButton *OpenButton;
	TButton *SaveButton;
	TButton *ClearButton;
	TOpenDialog *OpenDialog1;
	TSaveDialog *SaveDialog1;
	TRadioGroup *RadioGroup1;
	void __fastcall GetParametrsButtonClick(TObject *Sender);
	void __fastcall EncodeButtonClick(TObject *Sender);
	void __fastcall DecodeButtonClick(TObject *Sender);
	void __fastcall OpenButtonClick(TObject *Sender);
	void __fastcall SaveButtonClick(TObject *Sender);
	void __fastcall ClearButtonClick(TObject *Sender);
private:	// User declarations
	bool isKeyGenerated;
	unsigned long long p, q, r, phi, e, d;
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
	unsigned long long gcd(unsigned long long a, unsigned long long b);
	unsigned long long modPow(unsigned long long base, unsigned long long exp, unsigned long long mod);
	bool isPrime(unsigned long long n);
	unsigned long long modInverse(unsigned long long a, unsigned long long m);
	bool loadFileToMemo(const String &filename, bool isEncrypted);
	void encryptData(const std::vector<unsigned char> &data);
	void decryptData(const std::vector<unsigned short> &data);
	void addToMemo(TMemo *memo, const String &text);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
