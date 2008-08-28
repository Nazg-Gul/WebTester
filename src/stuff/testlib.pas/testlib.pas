{*

 * ================================================================================
 *  testlib.pas - part of the TestLib
 * ================================================================================
 *
 *  Pascal implementation for testlib.
 *
 *  Written (by Nazgul) under GPL.
 *
*}

unit testlib;

interface

const
  _OK = $0000;
  _WA = $0001;
  _PE = $0002;
  _CR = $0004;

const
  EOFChar = #26;

  EOLNChars = [#10, #13, EOFChar];
  blanks    = []; { Characters with ASCII code <= 32 are blanks by default }

  NumCharsBefore = [#10, #13, ' '];
  NumCharsAfter = [#10, #13, ' '];

  MaxStrLen = 255;

type

  Real=Extended; { !!! Warning !!! }

  TCharset = set of char;

  TMode = (TFM_INPUT, TFM_OUTPUT, TFM_ANSWER);

  TTestlibFile = object
    public
    constructor Create  (fn: string; m: TMode);
    destructor  Destroy;

    
    function ReadInteger: integer;    { Read integer value from file }
    function ReadLongint: longint;    { Read longint from file }
    function ReadFloat: real;         { Read double from file }

    
    function NextChar: char;     { Read char from file and move to next }
    function CurChar: char;      { Just return current char }

    function eof: boolean;      { Check for end of file }
    function eoln: boolean;     { Check for end of line }
    function seekeof: boolean;  { Seek for end of file }
    function seekeoln: boolean; { Seek for end on line }

    
    procedure NextLine;                { Goto next line }
    procedure skip (charset:TCharset); { Skip characters from set. Do not generate errors }

    function ReadWord (before, after: TCharset): string;

    function ReadString: string;

    private
    ch : char;
    stream: Text;
    mode: TMode;
    
    function ReadNumber: Real;
  end;

  procedure Quit (code: integer; desc: string);

var
  inf, ouf, ans: TTestlibFile;
  initialized: boolean;
  silent: boolean;

implementation

uses crt;

{**** TTestlibFile ****}

{ Constructor }
constructor TTestlibFile.Create (fn: string; m: TMode);
begin
  assign (stream, fn);

  {$i-}
  reset (stream);
  {$i+}

  m:=mode;

  if (ioResult<>0) then
    if m=TFM_OUTPUT
      then Quit (_PE, 'File not found: '+fn)
      else Quit (_CR, 'File not found: '+fn);

  if (system.eof (stream))
    then ch:=EOFChar
    else read (stream, ch);
end;

{ Destructor }
destructor TTestlibFile.Destroy;
begin

{  if (mode=TFM_OUTPUT) and (not eof) then
    Quit (_PE, 'Extra information in output file'); }

  {$i-}
  close (stream);
  {$i+}
end;

{ Read integer value from file }
function TTestlibFile.ReadInteger: integer;
var a: longint;
begin

  a:=trunc (ReadNumber);

  if (a<-32768) or (a>32767) then
    Quit (_PE, 'ReadInteger can work ONLY with integer values.');
  
  ReadInteger:=a;
  
end;

{ Read longint from file }
function TTestlibFile.ReadLongint: longint;
begin
  ReadLongInt:=trunc (ReadNumber);
end;
 
 { Read double from file }
function TTestlibFile.ReadFloat: real;
begin
  ReadFloat:=ReadNumber;
end;

{ Read char from file and move to next }
function TTestlibFile.NextChar: char;
begin
  NextChar:=CurChar;

  {$i-}
  read (stream, ch);
  {$i+}

  if IOResult<>0 then
    Quit (_CR, 'Disk read error');
end;

{ Just return current char }
function TTestlibFile.CurChar: char;
begin
  curchar:=ch;
end;

{ Check for end of file }
function TTestlibFile.eof: boolean;
begin
  eof:=CurChar=EOFChar;
end;

{ Seek for end of file }
function TTestlibFile.seekeof: boolean;
begin
  while ((ord (CurChar)<=32) or (CurChar in blanks)) and (CurChar<>EOFChar) do NextChar;
  seekeof:=CurChar=EOFChar;
end;
    
{ Check for end of line }
function TTestlibFile.eoln: boolean;
begin
  eoln:=CurChar in EOLNChars;
end;

{ Seek for end on line }
function TTestlibFile.seekeoln: boolean;
begin
  while (CurChar in blanks) and not (CurChar in EOLNChars) and not eof do NextChar;
  seekeoln:=CurChar in EOLNChars;
end;

{ Goto next line }
procedure TTestlibFile.NextLine;
begin
  while not (CurChar in EOLNChars) and not eof do NextChar;
  if CurChar=#13 then NextChar;
  if CurChar=#10 then NextChar;
end;

{ Skip characters from set. Do not generate errors }
procedure TTestlibFile.skip (charset: TCharset);
begin
  while (CurChar in charset) and not eof do
    NextChar;
end;

function TTestlibFile.ReadWord (before, after: TCharset): string;
var
  s: string;
begin
  skip (before);
  s:='';
  
  while not (CurChar in after) and not eof do
  begin
    if (length (s)>=MaxStrLen-1) then Quit (_PE, 'Word is too long');
    s:=s+CurChar;
    NextChar;
  end;
  skip (after);

  ReadWord:=s;
end;

function TTestlibFile.ReadNumber: Real;
var
  s: string;
  res: real;
  code: integer;

begin

  if seekeof then
    Quit (_PE, 'Unexpected end of file');

  s:=ReadWord (NumCharsBefore, NumCharsAfter);
  
  val (s, res, code);

  if (code<>0) then
  begin
    Quit (_PE, 'Wrong integer format');
  end;

  ReadNumber:=res;
end;

function TTestlibFile.ReadString: string;
var s: string;
begin
  s:='';

  if CurChar = EOFChar then
    Quit (_PE, 'Unexpected end of file');

  while not (CurChar in EOLNChars) do
  begin
    if (length (s)>MaxStrLen-1) then
      Quit (_PE, 'String to long');
    s:=s+CurChar;
    NextChar;
  end;

  if CurChar=#13 then NextChar;
  if CurChar=#10 then NextChar;

  ReadString:=s;
end;

{**** Some internal stuff ****}
procedure Quit (code: integer; desc: string);
begin

  {** Post-checking of output file **}
  if (initialized) and (code=_OK) and (not ouf.seekeof) then { CHeck for extra information }
  begin
    code:=_PE;
    desc:='Extra information in output file';
  end;

  { Set text color }
  if code<>_OK
    then TextColor (LightRed)
    else TextColor (LightGreen);

  { Write error message }
  case code of
    _OK: write ('OK');
    _WA: write ('WA');
    _PE: write ('PE');
    _CR: write ('CR');
  end;

  TextColor (LightGray);

  if desc<>'' then
    writeln ('  ', desc) else
    writeln;

  { For some sound messaging }
  if (code<>_OK) and (not silent) then
  begin
    sound (1000);
    delay (200);
    nosound;
  end;

  { Terminate executing of checker }
  halt (code);
end;


begin

  if (ParamCount<3) or (ParamCount>4) then
    Quit (_CR, 'Usage: checker <input file> <output file> <answer file> [-s]');

  if (ParamCount=4) then
    if (ParamStr (4)='-s') then silent:=true else 
      Quit (_CR, 'Usage: checker <input file> <output file> <answer file> [-s]');

  inf.Create (ParamStr (1), TFM_INPUT);
  ouf.Create (ParamStr (2), TFM_OUTPUT);
  ans.Create (ParamStr (3), TFM_ANSWER);

  initialized:=true;

end.
