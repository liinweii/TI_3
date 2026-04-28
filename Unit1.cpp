//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Unit1.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
	InputMemo->Clear();
    OutputMemo->Clear();
    RadioGroup1 = new TRadioGroup(this);
    RadioGroup1->Parent = this;
	RadioGroup1->Left = 15;
	RadioGroup1->Top = 100;
	RadioGroup1->Width = 220;
	RadioGroup1->Height = 70;
	RadioGroup1->Items->Add(L"File to encrypt");
	RadioGroup1->Items->Add(L"File to decrypt");
	RadioGroup1->ItemIndex = 0;

    isKeyGenerated = false;
    OpenDialog1 = new TOpenDialog(this);
    SaveDialog1 = new TSaveDialog(this);
    SaveDialog1->Filter = "All files|*.*";
    OpenDialog1->Filter = "All files|*.*";

	PEdit->Text = "";
	QEdit->Text = "";
	EEdit->Text = "";

}

unsigned long long TForm1::gcd(unsigned long long a, unsigned long long b)
{
    while (b != 0) {
        unsigned long long t = b;
		b = a % b;
        a = t;
    }
    return a;
}

unsigned long long TForm1::modPow(unsigned long long base, unsigned long long exp, unsigned long long mod)
{
	unsigned long long result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp & 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}

bool TForm1::isPrime(unsigned long long n)
{
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;

    for (unsigned long long i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

unsigned long long TForm1::modInverse(unsigned long long a, unsigned long long m)
{
    long long m0 = m, t, q;
    long long x0 = 0, x1 = 1;

    if (m == 1) return 0;

    while (a > 1) {
        q = a / m;
        t = m;
        m = a % m;
        a = t;
        t = x0;
        x0 = x1 - q * x0;
        x1 = t;
	}

	if (x1 < 0) x1 += m0;

	return x1;
}

bool TForm1::loadFileToMemo(const String &filename, bool isEncrypted)
{
 std::ifstream file(filename.c_str(), std::ios::binary);
    if (!file.is_open()) {
		Application->MessageBox(L"Open file error", L"Error", MB_OK);
        return false;
    }

    InputMemo->Clear();
    InputMemo->Lines->BeginUpdate();

    String line;
    int countInLine = 0;

	if (isEncrypted) {
        unsigned char lowByte, highByte;
        int count = 0;

        while (file.read(reinterpret_cast<char*>(&lowByte), sizeof(lowByte)) &&
               file.read(reinterpret_cast<char*>(&highByte), sizeof(highByte))) {
            unsigned short val = static_cast<unsigned short>(lowByte) |
                                (static_cast<unsigned short>(highByte) << 8);

            if (countInLine > 0) line += L" ";
            line += IntToStr(val);
            countInLine++;
            count++;

            if (countInLine >= 20) {
                InputMemo->Lines->Add(line);
                line = L"";
                countInLine = 0;
            }
        }

        file.close();

        if (line.Length() > 0) {
            InputMemo->Lines->Add(line);
        }

	} else {
		char ch;
		int count = 0;

        while (file.get(ch)) {
            unsigned char byte = static_cast<unsigned char>(ch);

            if (countInLine > 0) line += L" ";
            line += IntToStr(static_cast<int>(byte));
            countInLine++;
            count++;

            if (countInLine >= 20) {
                InputMemo->Lines->Add(line);
                line = L"";
                countInLine = 0;
            }
        }

        file.close();

        if (line.Length() > 0) {
			InputMemo->Lines->Add(line);
		}
    }

    InputMemo->Lines->EndUpdate();
	return true;
}

void TForm1::encryptData(const std::vector<unsigned char> &data)
{
OutputMemo->Clear();
	OutputMemo->Lines->BeginUpdate();

    String line;
    int countInLine = 0;

    for (size_t i = 0; i < data.size(); i++) {
        unsigned long long m = static_cast<unsigned long long>(data[i]);
        unsigned long long c = modPow(m, e, r);

        if (countInLine > 0) line += L" ";
        line += IntToStr(static_cast<int>(c));
        countInLine++;

		if (countInLine >= 20) {
            OutputMemo->Lines->Add(line);
            line = L"";
            countInLine = 0;
        }

    }

    if (line.Length() > 0) {
        OutputMemo->Lines->Add(line);
    }

	OutputMemo->Lines->EndUpdate();

	Application->MessageBox(L"Encrypted successfully", L"Success", MB_OK);
}


void TForm1::decryptData(const std::vector<unsigned short> &data)
{
	 OutputMemo->Clear();
	OutputMemo->Lines->BeginUpdate();
    String line;
    int countInLine = 0;

    for (size_t i = 0; i < data.size(); i++) {
        unsigned long long m = modPow(static_cast<unsigned long long>(data[i]), d, r);

        if (countInLine > 0) line += L" ";
        line += IntToStr(static_cast<int>(m));
        countInLine++;

        if (countInLine >= 20) {
            OutputMemo->Lines->Add(line);
            line = L"";
            countInLine = 0;
		}
	}

    if (line.Length() > 0) {
        OutputMemo->Lines->Add(line);
    }

    OutputMemo->Lines->EndUpdate();

	Application->MessageBox(L"Decrypted successfully", L"Success", MB_OK);
}


//---------------------------------------------------------------------------
void __fastcall TForm1::GetParametrsButtonClick(TObject *Sender)
{
	if (PEdit->Text.IsEmpty() || QEdit->Text.IsEmpty()) {
		Application->MessageBox(L"Enter p and q", L"Error", MB_OK);
		return;
	}

	p = StrToInt64(PEdit->Text);
	q = StrToInt64(QEdit->Text);

	if (!isPrime(p) || !isPrime(q)) {
		Application->MessageBox(L"p and q must be prime", L"Error", MB_OK);
        return;
    }

    if (p == q) {
		Application->MessageBox(L"p and q mustn't be equal", L"Error", MB_OK);
        return;
    }

    r = p * q;
	if (r < 256 || r > 65535) {
        Application->MessageBox(L"p * q must be between 256 and 65535", L"Error", MB_OK);
		return;
	}
    if (r < p || r < q) {
		Application->MessageBox(L"Too big p and q", L"Error", MB_OK);
		return;
    }

	phi = (p - 1) * (q - 1);

    if (DEdit->Text.IsEmpty()) {
		Application->MessageBox(L"Enter d", L"Error", MB_OK);
        return;
    }

    d = StrToInt64(DEdit->Text);

    if (d <= 1 || d >= phi) {
		Application->MessageBox(L"d must be between 1 and φ(r)", L"Error", MB_OK);
        return;
    }

    if (gcd(d, phi) != 1) {
		Application->MessageBox(L"d and φ(r) must be relatively prime", L"Error", MB_OK);
        return;
    }

    e = modInverse(d, phi);

    if (e <= 1 || e >= phi) {
		Application->MessageBox(L"Try another d.", L"Error", MB_OK);
        return;
    }

	REdit->Text = IntToStr(static_cast<int>(r));
	FEdit->Text = IntToStr(static_cast<int>(phi));
    EEdit->Text = IntToStr(static_cast<int>(e));

	isKeyGenerated = true;
}


//---------------------------------------------------------------------------

void TForm1::addToMemo(TMemo *memo, const String &text)
{
    memo->Lines->BeginUpdate();
    memo->Lines->Add(text);
    memo->Lines->EndUpdate();
}

void __fastcall TForm1::EncodeButtonClick(TObject *Sender)
{
	if (!isKeyGenerated) {
		Application->MessageBox(L"You need to generate key", L"Error", MB_OK);
		return;
    }

    if (InputMemo->Lines->Count == 0) {
		Application->MessageBox(L"No data to encrypt", L"Error", MB_OK);
        return;
    }

    std::vector<unsigned char> data;

    for (int i = 0; i < InputMemo->Lines->Count; i++) {
        String line = InputMemo->Lines->Strings[i];
        line = line.Trim();
        if (line.IsEmpty()) continue;

        int pos = 1;
        while (pos <= line.Length()) {
            while (pos <= line.Length() && line[pos] == L' ') pos++;
            if (pos > line.Length()) break;

            int start = pos;
            while (pos <= line.Length() && line[pos] != L' ') pos++;

            String token = line.SubString(start, pos - start);
            int val = StrToIntDef(token, -1);
            if (val >= 0 && val <= 255) {
                data.push_back(static_cast<unsigned char>(val));
            }
        }
    }

    if (data.empty()) {
		Application->MessageBox(L"No data to encrypt", L"Error", MB_OK);
        return;
    }

	encryptData(data);
}


//---------------------------------------------------------------------------

void __fastcall TForm1::DecodeButtonClick(TObject *Sender)
{
	if (!isKeyGenerated) {
		Application->MessageBox(L"You need to generate key", L"Error", MB_OK);
        return;
    }

	if (InputMemo->Lines->Count == 0) {
		Application->MessageBox(L"No data to decrypt", L"Error", MB_OK);
        return;
    }

    std::vector<unsigned short> data;

    for (int i = 0; i < InputMemo->Lines->Count; i++) {
        String line = InputMemo->Lines->Strings[i];
        line = line.Trim();
        if (line.IsEmpty()) continue;

        int pos = 1;
        while (pos <= line.Length()) {
            while (pos <= line.Length() && line[pos] == L' ') pos++;
            if (pos > line.Length()) break;

            int start = pos;
            while (pos <= line.Length() && line[pos] != L' ') pos++;

            String token = line.SubString(start, pos - start);
            int val = StrToIntDef(token, -1);
            if (val >= 0 && val <= 65535) {
                data.push_back(static_cast<unsigned short>(val));
            }
        }
    }

    if (data.empty()) {
		Application->MessageBox(L"No data to decrypt", L"Error", MB_OK);
        return;
    }

	decryptData(data);
}
//---------------------------------------------------------------------------

void __fastcall TForm1::OpenButtonClick(TObject *Sender)
{
OpenDialog1->Title = L"Choose file";
    OpenDialog1->Filter = L"All files|*.*";
	if (OpenDialog1->Execute()) {
		bool isEncrypted = (RadioGroup1->ItemIndex == 1);
		loadFileToMemo(OpenDialog1->FileName, isEncrypted);
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm1::SaveButtonClick(TObject *Sender)
{
 if (OutputMemo->Lines->Count == 0) {
		Application->MessageBox(L"No data to save", L"Error", MB_OK);
        return;
    }

	SaveDialog1->Title = L"Save data as";
    if (SaveDialog1->Execute()) {
        std::ofstream output(SaveDialog1->FileName.c_str(), std::ios::binary);
        if (!output.is_open()) {
			Application->MessageBox(L"Create file error", L"Error", MB_OK);
            return;
        }

        std::vector<unsigned long long> numbers;

        for (int i = 0; i < OutputMemo->Lines->Count; i++) {
            String line = OutputMemo->Lines->Strings[i];
            line = line.Trim();
            if (line.IsEmpty()) continue;

            int pos = 1;
            while (pos <= line.Length()) {
                while (pos <= line.Length() && line[pos] == L' ') pos++;
                if (pos > line.Length()) break;

                int start = pos;
                while (pos <= line.Length() && line[pos] != L' ') pos++;

                String token = line.SubString(start, pos - start);
                unsigned long long val = StrToInt64Def(token, 0);
                numbers.push_back(val);
            }
        }

        bool isEncrypted = false;
        for (unsigned long long val : numbers) {
            if (val > 255) {
                isEncrypted = true;
                break;
            }
        }

        if (isEncrypted) {
            for (unsigned long long val : numbers) {
                unsigned short val16 = static_cast<unsigned short>(val);
                unsigned char lowByte = val16 & 0xFF;
                unsigned char highByte = (val16 >> 8) & 0xFF;
                output.write(reinterpret_cast<char*>(&lowByte), sizeof(lowByte));
                output.write(reinterpret_cast<char*>(&highByte), sizeof(highByte));
            }
        } else {
            for (unsigned long long val : numbers) {
                unsigned char byte = static_cast<unsigned char>(val);
                output.write(reinterpret_cast<char*>(&byte), sizeof(byte));
            }
        }

		output.close();
		Application->MessageBox(L"Data saved", L"Success", MB_OK);
    }
}
//---------------------------------------------------------------------------

void __fastcall TForm1::ClearButtonClick(TObject *Sender)
{
	QEdit->Text = "";
	PEdit->Text = "";
	DEdit->Text = "";
	EEdit->Text = "";
	REdit->Text = "";
    FEdit->Text = "";
	InputMemo->Clear();
	OutputMemo->Clear();
}
//---------------------------------------------------------------------------


