#include <vector>
#include <stack>
#include <sstream>
#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <memory.h>
#include <unistd.h>
#include <locale>
#include <codecvt>
#include <string>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <math.h>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr를 포함하는 헤더
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using uint = unsigned int;
using ushort = unsigned short;
using uchar = unsigned char;

vector<uchar *> CoRe;
vector<vector<tuple<int, ushort, long long>>> order;
bool server = true;
uint cNode[10];
ushort cCh[10];
uint pageNum[10];
pair<uint, ushort> copy;
int line_spacing = 5;
int text_size = 15;
vector<wstring> LogStr{};
uint gar = 39701;
uint ttt = 166972;
vector<uchar> zero8 = {0, 0, 0, 0, 0, 0, 0, 0};
uchar initValues[18] = {14, 0, 0, 0, 1, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // node 추가시 초기값 설정

ushort charToushort(uchar *arr)
{
    return (static_cast<ushort>(arr[0])) |
           (static_cast<ushort>(arr[1]) << 8);
}
ushort numCh(uint node) // node의 채널 수를 반환(채널 수는 2바이트로 저장됨)
{
    return (static_cast<ushort>(CoRe[node][4])) |
           (static_cast<ushort>(CoRe[node][5]) << 8);
}
uint charTouint(uchar *arr)
{
    return (static_cast<unsigned int>(arr[0])) |
           (static_cast<unsigned int>(arr[1]) << 8) |
           (static_cast<unsigned int>(arr[2]) << 16) |
           (static_cast<unsigned int>(arr[3]) << 24);
}
pair<uint, ushort> charToPair(uchar *arr)
{
    unsigned int uint_val =
        (static_cast<unsigned int>(arr[0])) |
        (static_cast<unsigned int>(arr[1]) << 8) |
        (static_cast<unsigned int>(arr[2]) << 16) |
        (static_cast<unsigned int>(arr[3]) << 24);

    ushort ushort_val =
        (static_cast<ushort>(arr[4])) |
        (static_cast<ushort>(arr[5]) << 8);

    return {uint_val, ushort_val};
}
uchar *ushortToBytes(ushort val1)
{
    unsigned char *bytes = new unsigned char[2]; // 2 바이트 배열 동적 할당
    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    return bytes; // 배열의 포인터 반환
}
uchar *uintToBytes(uint val1)
{
    unsigned char *bytes = new unsigned char[4]; // 4 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((val1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((val1 >> 24) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *uintToBytes2(uint val1, uint val2)
{
    unsigned char *bytes = new unsigned char[8]; // 8 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((val1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((val1 >> 24) & 0xFF);
    bytes[4] = static_cast<unsigned char>(val2 & 0xFF);
    bytes[5] = static_cast<unsigned char>((val2 >> 8) & 0xFF);
    bytes[6] = static_cast<unsigned char>((val2 >> 16) & 0xFF);
    bytes[7] = static_cast<unsigned char>((val2 >> 24) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *pairToBytes(uint32_t val1, uint16_t val2)
{
    unsigned char *bytes = new unsigned char[6]; // 6 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(val1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((val1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((val1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((val1 >> 24) & 0xFF);
    bytes[4] = static_cast<unsigned char>(val2 & 0xFF);
    bytes[5] = static_cast<unsigned char>((val2 >> 8) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *pairToBytes2(uint node1, ushort ch1, uint node2, ushort ch2)
{
    unsigned char *bytes = new unsigned char[12]; // 6 바이트 배열 동적 할당

    bytes[0] = static_cast<unsigned char>(node1 & 0xFF);
    bytes[1] = static_cast<unsigned char>((node1 >> 8) & 0xFF);
    bytes[2] = static_cast<unsigned char>((node1 >> 16) & 0xFF);
    bytes[3] = static_cast<unsigned char>((node1 >> 24) & 0xFF);
    bytes[4] = static_cast<unsigned char>(ch1 & 0xFF);
    bytes[5] = static_cast<unsigned char>((ch1 >> 8) & 0xFF);
    bytes[6] = static_cast<unsigned char>(node2 & 0xFF);
    bytes[7] = static_cast<unsigned char>((node2 >> 8) & 0xFF);
    bytes[8] = static_cast<unsigned char>((node2 >> 16) & 0xFF);
    bytes[9] = static_cast<unsigned char>((node2 >> 24) & 0xFF);
    bytes[10] = static_cast<unsigned char>(ch2 & 0xFF);
    bytes[11] = static_cast<unsigned char>((ch2 >> 8) & 0xFF);

    return bytes; // 배열의 포인터 반환
}
uchar *chVec(uint node, ushort ch)
{
    uint posi = charTouint(CoRe[node] + 6 + 4 * ch);
    uint posiEnd;

    if (ch < numCh(node) - 1)
    {
        posiEnd = charTouint(CoRe[node] + 6 + 4 * (ch + 1));
    }
    else
    {
        posiEnd = 4 + charTouint(CoRe[node]);
    }
    uchar *result = new uchar[posiEnd - posi];
    for (int i = 0; i < posiEnd - posi; i++)
    {
        result[i] = CoRe[node][i + posi];
    }
    return result;
}
uchar *axis1(uint node, ushort ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    return CoRe[node] + startCoo + 4; // 포인터를 그대로 전달함.
}
uchar *axis2(uint node, ushort ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    return CoRe[node] + startRev + 4;
}
uchar *axis(uint node, ushort ch, ushort axis){
    uint startAxis = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeAxis = 0;
    for (int i = 2; i <= axis; i++){
        sizeAxis = charTouint(CoRe[node] + startAxis);
        startAxis += 4 + sizeAxis;
    }
    return CoRe[node] + startAxis + 4;
}
void changeShort(uchar *arr, uint index, ushort update)
{
    arr[index] = static_cast<uchar>(update & 0xFF);
    arr[index + 1] = static_cast<uchar>((update >> 8) & 0xFF);
}
void changeInt(uchar *arr, uint index, uint update)
{
    arr[index] = static_cast<uchar>(update & 0xFF);
    arr[index + 1] = static_cast<uchar>((update >> 8) & 0xFF);
    arr[index + 2] = static_cast<uchar>((update >> 16) & 0xFF);
    arr[index + 3] = static_cast<uchar>((update >> 24) & 0xFF);
}
void changePair(uchar *arr, uint index, uint node, ushort ch)
{
    // node 값을 벡터에 할당
    arr[index] = static_cast<uchar>(node & 0xFF);
    arr[index + 1] = static_cast<uchar>((node >> 8) & 0xFF);
    arr[index + 2] = static_cast<uchar>((node >> 16) & 0xFF);
    arr[index + 3] = static_cast<uchar>((node >> 24) & 0xFF);

    // ch 값을 벡터에 할당
    arr[index + 4] = static_cast<uchar>(ch & 0xFF);
    arr[index + 5] = static_cast<uchar>((ch >> 8) & 0xFF);
}
void eraseCoo(uint node, ushort ch, uint index)
{
    // 6바이트 제거
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    vector<uchar> replace = vector<uchar>(CoRe[node] + startCoo + 4 + 6 * (index + 1), CoRe[node] + startRev + 4 + sizeRev);
    for (int i = startCoo + 4 + 6 * index; i < startRev - 2 + sizeRev; i++)
    {
        CoRe[node][i] = replace[i - startCoo - 4 - 6 * index]; // Rev 위치를 앞으로 6칸 이동
    }
    changeInt(CoRe[node], startCoo, sizeCoo - 6);
}
void eraseRev(uint node, ushort ch, uint index)
{
    // 6바이트 제거
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = charTouint(CoRe[node] + startCoo + 4 + sizeCoo);
    // CoRe[node].erase(CoRe[node].begin() + startRev + 4 + 6 * index, CoRe[node].begin() + startCoo + 4 + 6 * (index + 1));
    changePair(CoRe[node], startRev + 4 + 6 * index, 0, 0); // 제거할 때 메모리를 삭제시키지 않고 그냥 0으로 채움.(나중에 재활용하기 위함 + 프로그램 성능을 위해)
    uint sizeRev = charTouint(CoRe[node] + startRev);
    changeInt(CoRe[node], startRev, sizeRev - 6);
    // uint sizeCh = charTouint(vec, startCoo - 4);
    // changeInt(vec, startCoo - 4, sizeRev - 6);
}
void clearCoo(uint node, uint ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    changeInt(CoRe[node], startCoo, 0);
    vector<uchar> replace = vector<uchar>(CoRe[node] + startRev, CoRe[node] + startRev + 4 + sizeRev);
    for (int i = 4; i < 8 + sizeRev; i++)
    {
        CoRe[node][i] = replace[i - 4];
    }
    // vec.erase(vec.begin() + 4, vec.begin() + 4 + szC);
    // for (int i = 0; i < 4; i++)
    // {
    //     vec[i] = 0;
    // }
}
void clearRev(uint node, uint ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    changeInt(CoRe[node], startRev, 0);
    // uint szC = charTouint(vec, 0);
    // vec = vector<uchar>(vec.begin(), vec.begin() + 8 + szC);
    // for (int i = 0; i < 4; i++)
    // {
    //     vec[4 + szC + i] = 0;
    // }
}
void insertArr(uint node, uint index, uchar *add, uint addSize)
{
    uint sizeArr = 4 + charTouint(CoRe[node]);
    uchar *result = new uchar[sizeArr + addSize];
    changeInt(result, 0, sizeArr + addSize - 4);
    for (uint i = 4; i < index; ++i)
    {
        result[i] = CoRe[node][i];
    }
    for (uint i = 0; i < addSize; ++i)
    {
        result[index + i] = add[i];
    }
    for (uint i = index; i < sizeArr; ++i)
    {
        result[i + addSize] = CoRe[node][i];
    }
    delete[] CoRe[node]; // 메모리 해제 먼저 해야 함!
    CoRe[node] = result;
}
void pushCoo(uint node, ushort ch, uchar *add)
{
    ushort numCh = charToushort(CoRe[node] + 4);
    uint startCoo = charTouint(CoRe[node] + (6 + 4 * ch));
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    uint startCoo2 = 0;
    if (ch < numCh - 1)
    {
        startCoo2 = charTouint(CoRe[node] + 6 + 4 * (ch + 1));
    }
    else
    {
        // startCoo2 = CoRe[node].size();
        startCoo2 = 4 + charTouint(CoRe[node]);
    }
    if (sizeCoo + sizeRev + 14 > startCoo2 - startCoo)
    {
        for (ushort i = ch + 1; i < numCh; i++)
        {
            uint temp = charTouint(CoRe[node] + 6 + 4 * i);
            changeInt(CoRe[node], 6 + 4 * i, temp + 6);
        }
        insertArr(node, startRev, add, 6);
        // CoRe[node].insert(CoRe[node].begin() + startCoo + 4 + sizeCoo, add.begin(), add.end());
        changeInt(CoRe[node], startCoo, sizeCoo + 6);
    }
    else
    {
        changeInt(CoRe[node], startCoo, sizeCoo + 6);
        vector<uchar> RevVec = vector<uchar>(CoRe[node] + startRev, CoRe[node] + startRev + 4 + sizeRev);
        for (int i = startRev + 6; i < startRev + 10 + sizeRev; i++)
        {
            CoRe[node][i] = RevVec[i - startRev - 6];
        }
        for (int i = startRev; i < startRev + 6; i++)
        {
            CoRe[node][i] = add[i - startRev];
        }
    }
    delete[] add;
}
void pushRev(uint node, ushort ch, uchar *add)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    uint endPosi = startRev + 4 + sizeRev;
    ushort numCh = charToushort(CoRe[node] + 4);
    uint startCoo2 = 0;
    if (ch < numCh - 1)
    {
        startCoo2 = charTouint(CoRe[node] + 6 + 4 * (ch + 1));
    }
    else
    {
        // startCoo2 = CoRe[node].size();
        startCoo2 = 4 + charTouint(CoRe[node]);
    }
    if (sizeCoo + sizeRev + 14 > startCoo2 - startCoo)
    {
        // CoRe[node].insert(CoRe[node].begin() + endPosi, add.begin(), add.end());
        insertArr(node, endPosi, add, 6);
        for (ushort i = ch + 1; i < numCh; i++)
        {
            uint temp = charTouint(CoRe[node] + 6 + 4 * i);
            changeInt(CoRe[node], 6 + 4 * i, temp + 6);
        }
    }
    else
    {
        for (int i = endPosi; i < endPosi + 6; i++)
        {
            CoRe[node][i] = add[i - endPosi];
        }
    }
    delete[] add;
    changeInt(CoRe[node], startRev, sizeRev + 6);
}
void pushRev2(uint node, ushort ch, uchar *add) // Brain 함수에서만 사용하는 특수용도(일반적으로 사용하면 안됨)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    uint endPosi = startRev + 4 + sizeRev;
    ushort numCh = charToushort(CoRe[node] + 4);
    // CoRe[node].insert(CoRe[node].begin() + endPosi, add.begin(), add.end());
    insertArr(node, endPosi, add, 12);
    for (ushort i = ch + 1; i < numCh; i++)
    {
        changeInt(CoRe[node], 6 + 4 * i, charTouint(CoRe[node] + 6 + 4 * i) + 12);
    }
    changeInt(CoRe[node], startRev, sizeRev + 12);
}
void insertCoo(uint node, ushort ch, int indexCoo, uchar *add)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    insertArr(node, startCoo + 4 + indexCoo, add, 6);
    // CoRe[node].insert(CoRe[node].begin() + startCoo + 4 + indexCoo, add.begin(), add.end());
    changeInt(CoRe[node], startCoo, sizeCoo + 6);
    ushort sizeCh = numCh(node);
    for (ushort i = ch + 1; i < sizeCh; i++)
    {
        uint temp = charTouint(CoRe[node] + 6 + 4 * i);
        changeInt(CoRe[node], 6 + 4 * i, temp + 6);
    }
}
void insertRev(uint node, ushort ch, int indexRev, uchar *add)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    insertArr(node, startRev + +indexRev, add, 6);
    // CoRe[node].insert(CoRe[node].begin() + startRev + 4 + indexRev, add.begin(), add.end());
    changeInt(CoRe[node], startRev, sizeRev + 6);
    ushort sizeCh = numCh(node);
    for (ushort i = ch + 1; i < sizeCh; i++)
    {
        uint temp = charTouint(CoRe[node] + 6 + 4 * i);
        changeInt(CoRe[node], 6 + 4 * i, temp + 6);
    }
}
uint startCh(uint node, ushort ch)
{
    return charTouint(CoRe[node] + 6 + 4 * ch);
}
uint sizeCoo(uint node, ushort ch)
{
    uint start = startCh(node, ch);
    return charTouint(CoRe[node] + start);
}
uint sizeRev(uint node, ushort ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    return charTouint(CoRe[node] + startCoo + 4 + sizeCoo);
}
vector<uint> sizeCoRe(uint node, ushort ch)
{
    vector<uint> re(2);
    uint startCoo = startCh(node, ch);
    re[0] = charTouint(CoRe[node] + startCoo);
    re[1] = charTouint(CoRe[node] + startCoo + 4 + re[0]);
    return re;
}
void CoMove(uint ori, ushort oriCh, uint From, uint To, ushort ToCh)
{
    uchar *ToCoo = new uchar[6];
    uint start = startCh(ori, oriCh);
    for (int i = 0; i < charTouint(CoRe[ori] + start) / 6; i++)
    {
        uint ii = 4 + 6 * i;
        if (charTouint(CoRe[ori] + start + ii) == From)
        {
            std::copy(CoRe[ori] + start + ii, CoRe[ori] + start + ii + 6, ToCoo);
            eraseCoo(ori, oriCh, i);
        }
    }
    ushort revCh = charToushort(ToCoo + 4);
    pushCoo(To, ToCh, ToCoo); // pushCoo에서 ToCoo를 삭제함.
    vector<uint> szCR = sizeCoRe(From, revCh);
    start = startCh(From, revCh);
    for (int i = 0; i < szCR[1] / 6; i++)
    {
        if (charTouint(CoRe[From] + start + 8 + szCR[0] + 6 * i) == ori)
        {
            changePair(CoRe[From], start + 8 + szCR[0] + 6 * i, To, ToCh);
            break;
        }
    }
}
void link(uint prevNode, ushort prevCh, uint nextNode, ushort nextCh)
{
    pushCoo(prevNode, prevCh, pairToBytes(nextNode, nextCh));
    pushRev(nextNode, nextCh, pairToBytes(prevNode, prevCh));
}
std::wstring ushortToWstring(ushort num)
{
    wchar_t wc = static_cast<wchar_t>(num);
    wstring ws(1, wc);
    return ws;
}
std::wstring intToWString(int num)
{
    std::wstringstream wss;
    wss << num;
    return wss.str();
}
std::wstring utf8ToWstring(const std::string &utf8Str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(utf8Str);
}
std::wstring charToWstring(uchar *val) // size 정보 담아서 전달되어야 함.
{
    wstring re = L"";
    vector<unsigned short> ss;
    uint size = charTouint(val);
    for (int i = 0; i < size / 2; i++)
    {
        ss.push_back(charToushort(val + 4 + 2 * i));
    }
    // delete[] val;
    for (int i = 0; i < ss.size(); i++)
    {
        wstring temp = ushortToWstring(ss[i]);
        re.append(temp);
    }
    return re;
}
std::u32string utf8ToUtf32(const std::string &utf8Str)
{
    std::u32string utf32Text;

    for (size_t i = 0; i < utf8Str.length();)
    {
        unsigned char lead = utf8Str[i];
        int charLength = 1;
        char32_t charCode = 0;

        if (lead < 0x80)
        {
            charCode = lead;
        }
        else if ((lead >> 5) == 0x06)
        { // 2-byte character
            charLength = 2;
            charCode = lead & 0x1F;
        }
        else if ((lead >> 4) == 0x0E)
        { // 3-byte character
            charLength = 3;
            charCode = lead & 0x0F;
        }
        else if ((lead >> 3) == 0x1E)
        { // 4-byte character
            charLength = 4;
            charCode = lead & 0x07;
        }
        else
        {
            throw std::runtime_error("Invalid UTF-8 lead byte");
        }

        if (i + charLength > utf8Str.length())
        {
            throw std::runtime_error("Incomplete UTF-8 character sequence");
        }

        for (int j = 1; j < charLength; ++j)
        {
            unsigned char follow = utf8Str[i + j];
            if ((follow >> 6) != 0x02)
            {
                throw std::runtime_error("Invalid UTF-8 follow byte");
            }
            charCode = (charCode << 6) | (follow & 0x3F);
        }

        utf32Text.push_back(charCode);
        i += charLength;
    }
    return utf32Text;
}
std::string wstringToUtf8(const std::wstring &wstr)
{
    if (wstr.empty())
        return std::string();

    size_t sizeNeeded = wcstombs(nullptr, wstr.c_str(), 0);
    std::vector<char> buffer(sizeNeeded + 1);
    wcstombs(buffer.data(), wstr.c_str(), buffer.size());
    return std::string(buffer.data());
}
struct BitmapGlyphData
{
    unsigned char *bitmap; // 비트맵 데이터
    uint width;            // 비트맵 너비
    uint height;           // 비트맵 높이
    glm::ivec2 bearing;    // 비트맵 베어링
    uint advance;          // 다음 글리프까지의 이동 거리
};
std::map<uint32_t, BitmapGlyphData> bitmapGlyphMap;
GLuint Shader_2d;
void RenderGlyph(const BitmapGlyphData &glyph, float x, float y)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 텍스처 생성 및 바인딩
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 텍스처 데이터의 정렬 방식을 설정합니다. 반드시 필요!!(글리프 데이터의 너비가 2의 거듭제곱이 아닐 경우, 텍스처 데이터의 정렬 방식을 설정하지 않으면 텍스처가 깨집니다.)

    // 텍스처 이미지 업로드
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph.width, glyph.height, 0, GL_RED, GL_UNSIGNED_BYTE, glyph.bitmap);

    // 텍스처 파라미터 설정
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // 렌더링을 위한 쿼드 생성
    float vertices[6][4] = {
        {x, y + glyph.height, 0.0f, 0.0f},
        {x, y, 0.0f, 1.0f},
        {x + glyph.width, y, 1.0f, 1.0f},

        {x, y + glyph.height, 0.0f, 0.0f},
        {x + glyph.width, y, 1.0f, 1.0f},
        {x + glyph.width, y + glyph.height, 1.0f, 0.0f}};

    // VBO(버텍스 버퍼 객체) 및 VAO(버텍스 어레이 객체) 생성 및 바인딩
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // 버텍스 속성 설정
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glUseProgram(Shader_2d);

    // 색상 및 텍스처 유닛 설정
    glUniform3f(glGetUniformLocation(Shader_2d, "textColor"), 0.0f, 0.0f, 0.0f);
    glUniform1i(glGetUniformLocation(Shader_2d, "text"), 0);

    // 쿼드 렌더링
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // 리소스 정리
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &texture);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}
void RenderText(const std::string &text, float x, float y)
{
    glUseProgram(Shader_2d);

    std::u32string utf32Text = utf8ToUtf32(text);
    for (auto c : utf32Text)
    {
        auto glyphIter = bitmapGlyphMap.find(c);
        BitmapGlyphData glyph;

        if (glyphIter != bitmapGlyphMap.end())
        {
            glyph = glyphIter->second;
        }
        else
        {
            // 글리프 데이터를 찾을 수 없는 경우, 물음표 글리프를 사용합니다.
            LogStr.push_back(L"Glyph not found: " + intToWString(c));
            auto questionMarkIter = bitmapGlyphMap.find(U'?');
            if (questionMarkIter == bitmapGlyphMap.end())
            {
                throw std::runtime_error("Question mark glyph not found in bitmapGlyphMap");
            }
            glyph = questionMarkIter->second;
        }
        float xpos = x + glyph.bearing.x;
        float ypos = y + glyph.bearing.y - glyph.height;
        // OpenGL을 사용하여 비트맵을 화면에 렌더링합니다.
        RenderGlyph(glyph, xpos, ypos);
        x += (glyph.advance >> 6); // 비트맵의 다음 글리프 위치로 이동
    }
    glUseProgram(0);
}
vector<wstring> splitWstring(const wstring &input, const wstring &del)
{
    std::wistringstream wss(input);
    std::wstring token;
    std::vector<std::wstring> tokens;

    while (std::getline(wss, token, del[0]))
    {
        tokens.push_back(token);
    }

    return tokens;
}
std::vector<std::string> splitStringASCII(const std::string &input, char delimiter)
{
    std::istringstream iss(input);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(iss, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}
float stringAdvance(const std::string &text)
{
    float advance = 0;
    std::u32string utf32Text = utf8ToUtf32(text);

    for (auto c : utf32Text)
    {
        auto glyphIter = bitmapGlyphMap.find(c);
        if (glyphIter == bitmapGlyphMap.end())
        {
            continue;
        }
        BitmapGlyphData glyph = glyphIter->second;
        advance += (glyph.advance >> 6);
    }
    return advance;
}
std::vector<std::string> splitLines(string &input)
{
    const int maxWidth = 400;
    std::vector<std::string> lines;

    size_t start = 0, end = 0;
    float width = 0;
    auto utf8CharLength = [](unsigned char byte) -> size_t
    {
        if (byte >= 0xF0)
            return 4;
        else if (byte >= 0xE0)
            return 3;
        else if (byte >= 0xC0)
            return 2;
        return 1;
    };

    while (end < input.size())
    {
        unsigned char byte = static_cast<unsigned char>(input[end]);
        size_t charLength = utf8CharLength(byte);
        string currentChar = input.substr(end, charLength);
        if (input[end] == '\n')
        {
            lines.push_back(input.substr(start, end - start + 1));
            start = ++end;
            width = 0;
            continue;
        }
        float advance = stringAdvance(currentChar);
        width = width + advance;

        if (width > maxWidth)
        {
            lines.push_back(input.substr(start, end - start));
            start = end;
            width = 0;
        }
        else
        {
            end += charLength;
            if (end >= input.size())
            {
                lines.push_back(input.substr(start));
            }
        }
    }
    return lines;
}
std::vector<std::wstring> splitLinesW(const std::wstring &input)
{
    const int maxWidth = 400;
    std::vector<std::wstring> lines;

    size_t start = 0;
    float width = 0;

    for (size_t end = 0; end <= input.size(); ++end)
    {
        if (end == input.size() || input[end] == L'\n')
        {
            lines.push_back(input.substr(start, end - start));
            start = end + 1;
            width = 0;
        }
        else
        {
            float advance = bitmapGlyphMap[input[end]].advance >> 6;
            width += advance;
            if (width > maxWidth)
            {
                lines.push_back(input.substr(start, end - start));
                start = end;
                width = 0;
            }
        }
    }
    return lines;
}
void Log(wstring text)
{
    if (text != L"")
    {
        LogStr.push_back(text);
    }
    int line = 0;
    int size = LogStr.size();
    if (size > 15)
    { // 15줄만 출력하기
        line = size - 15;
    }
    uchar ii = 0;
    for (int i = line; i < LogStr.size(); i++)
    {
        std::vector<std::wstring> lines = splitWstring(LogStr[i], L"\n");
        for (int j = 0; j < lines.size(); j++)
        {
            std::vector<wstring> lines2 = splitLinesW(lines[j]);
            for (const auto &line2 : lines2)
            {
                ii++;
                string str = wstringToUtf8(line2);
                RenderText(str, 1000, 1000 - (500 + 15 * ii));
            }
        }
    }
}
uchar *wstringToUChar(const std::wstring &str)
{
    // 동적 배열 할당
    unsigned char *result = new unsigned char[str.size() * 2 + 4];
    unsigned char *size = uintToBytes(2 * str.size());
    result[0] = size[0];
    result[1] = size[1];
    result[2] = size[2];
    result[3] = size[3];
    delete[] size;
    int index = 4;
    for (wchar_t c : str)
    {
        unsigned short ushort_c = static_cast<unsigned short>(c);
        result[index++] = static_cast<unsigned char>(ushort_c & 0xFF);
        result[index++] = static_cast<unsigned char>((ushort_c >> 8) & 0xFF);
    }

    return result;
}
uchar *word(uint node)
{
    // uchar *re1 = new uchar[1024];
    uchar re1[4000]; // 스택에 고정 크기 할당

    ushort nowPosi = 4;
    if (node >= ttt && node < ttt + 256)
    {
        re1[nowPosi] = (uchar)(node - ttt);
        nowPosi++;
    }
    else if (sizeRev(node, 0) > 0)
    {
        stack<uint> st;
        vector<uint> index;
        st.push(node);
        index.push_back(0);

        int ii = 0;
        while (!st.empty() && ii < 4000)
        {
            ii++;
            uint topNode = st.top();
            vector<uint> szCR = sizeCoRe(topNode, 0);
            if (szCR[1] == 6 * index.back())
            {
                if (st.size() == 1)
                    break;
                st.pop();
                index.pop_back();
                index.back()++;
                continue;
            }
            uint startCoo = startCh(topNode, 0);
            uint rev = charTouint(CoRe[topNode] + startCoo + 8 + szCR[0] + 6 * index.back());
            if (rev != 41155)
            {
                st.push(rev);
                index.push_back(0);
            }
            else
            {
                uchar chch = (uchar)(topNode - ttt);
                re1[nowPosi] = chch;
                nowPosi++;
                st.pop();
                index.pop_back();
                if (!index.empty())
                    index.back()++;
            }
        }
        if (ii > 4000) // 오류 처리 logic은 while 문 밖으로 꺼내는 게 좋음
        {
            Log(L"word Error! node: " + intToWString(node));
        }
    }
    uchar *sizeRe = uintToBytes(nowPosi - 4);
    re1[0] = sizeRe[0];
    re1[1] = sizeRe[1];
    re1[2] = sizeRe[2];
    re1[3] = sizeRe[3];
    delete[] sizeRe;
    // 동적 배열에 필요한 크기만큼만 할당
    uchar *result = new uchar[nowPosi];
    std::copy(re1, re1 + nowPosi, result); // std::copy를 사용하여 re1의 내용을 result로 복사

    return result;
}
uchar *Sheet(uint node)
{
    constexpr uint reSize = 4000;
    uchar *re = new uchar[reSize]();
    ushort nowPosi = 4;

    if (node == 41155)
    {
        delete[] re;
        return wstringToUChar(L"Entrance");
    }

    int szR = sizeRev(node, 0);
    if (szR > 1)
    {
        delete[] re;
        return word(node);
    }
    else if (szR == 0)
    {
        std::pair<uint, ushort> dd(node, 0);
        uint start1 = startCh(dd.first, dd.second);
        dd = charToPair(CoRe[dd.first] + start1 + 4);

        while (dd.first != node || dd.second != 0)
        {
            uchar *wordDD = word(dd.first); // Assuming word() allocates memory
            uint sizeWord = charTouint(wordDD);

            if (nowPosi + sizeWord <= reSize)
            {
                memcpy(re + nowPosi, wordDD + 4, sizeWord);
                nowPosi += sizeWord;
            }

            uint startCoo = startCh(dd.first, dd.second);
            if (charTouint(CoRe[dd.first] + startCoo) == 0)
            {
                delete[] wordDD;
                break;
            }
            dd = charToPair(CoRe[dd.first] + startCoo + 4);
            delete[] wordDD;
        }
    }

    uchar *sizeRe = uintToBytes(nowPosi - 4);
    memcpy(re, sizeRe, 4); // Copy 4 bytes of size
    delete[] sizeRe;

    return re;
}
bool startsWith(const std::wstring &str, const std::wstring &prefix)
{
    if (str.size() < prefix.size())
    {
        return false;
    }
    return std::equal(prefix.begin(), prefix.end(), str.begin());
}
vector<uint> tokenize(vector<uchar> data)
{
    uint coord = 41155;
    ushort ch = 0;
    vector<uint> re;
    while (data.size() > 0) // str이 빌 때까지 반복
    {
        bool check2 = false;
        uint startCoo = startCh(coord, 0);
        for (int i = 0; i < charTouint(CoRe[coord] + startCoo) / 6; i++)
        {
            uint cd = charTouint(CoRe[coord] + startCoo + 4 + 6 * i);
            uchar *wordcd = word(cd);
            uint sizeWordCd = charTouint(wordcd);
            if (std::equal(wordcd + 4, wordcd + 4 + sizeWordCd, data.begin())) // 기존 글자 존재할 경우 추가 글자 확인
            {
                coord = cd;
                check2 = true;
                break;
            }
        }
        if (!check2)
        {
            if (coord != 41155)
            {
                re.push_back(coord);
                uchar *ws = word(coord);
                vector<uchar> temp(data.begin() + charTouint(ws), data.end());
                data = temp;
                coord = 41155;
            }
        }
    }
    return re;
}
std::vector<uchar> combineVector(const std::vector<uchar> &vec1, const std::vector<uchar> &vec2)
{
    std::vector<uchar> result;
    result.reserve(vec1.size() + vec2.size()); // 미리 메모리를 할당하여 효율성을 높입니다.
    result.insert(result.end(), vec1.begin(), vec1.end());
    result.insert(result.end(), vec2.begin(), vec2.end());
    return result;
}
uchar *combineArr(uchar *arr1, uchar *arr2)
{
    uint size1 = charTouint(arr1);
    uint size2 = charTouint(arr2);
    uchar *result = new uchar[4 + size1 + size2]; // arr1 and arr2 must contain size infomation.
    uchar *sizeRe = uintToBytes(size1 + size2);
    result[0] = sizeRe[0];
    result[1] = sizeRe[1];
    result[2] = sizeRe[2];
    result[3] = sizeRe[3];
    delete[] sizeRe;
    for (int i = 4; i < 4 + size1; i++)
    {
        result[i] = arr1[i];
    }
    for (int i = 4 + size1; i < 4 + size1 + size2; i++)
    {
        result[i] = arr2[i - size1];
    }
    return result;
}
bool areEqual(uchar *arr1, uchar *arr2, uint size)
{
    for (int i = 0; i < size; i++)
    {
        if (arr1[i] != arr2[i])
        {
            return false;
        }
    }
    return true;
}
uint firstToken(uchar *data, uint size)
{
    bool check2 = true;
    uint coord = 41155;
    while (check2)
    {
        check2 = false;
        uint startCoo = charTouint(CoRe[coord] + 6);
        uint sizeCoo = charTouint(CoRe[coord] + startCoo);
        for (int i = 0; i < sizeCoo / 6; i++)
        {
            uint cd = charTouint(CoRe[coord] + startCoo + 4 + 6 * i);
            uchar *wordcd = word(cd); // 사이즈 포함된 값임
            uint sizeWord = charTouint(wordcd);
            if (areEqual(wordcd + 4, data, sizeWord)) // 기존 글자 존재할 경우 추가 글자 확인
            {
                coord = cd;
                if (sizeWord >= size)
                {
                    check2 = true;
                }
                delete[] wordcd;
                break;
            }
            delete[] wordcd;
        }
    }
    return coord;
}
void clearCh(uint node, ushort ch)
{
    uint startCoo = startCh(node, ch);
    std::fill(CoRe[node] + startCoo, CoRe[node] + startCoo + 8, 0);
}
void addCh(uint node)
{
    ushort numch = numCh(node);
    changeShort(CoRe[node], 4, numch + 1);
    uint sizeNode = 4 + charTouint(CoRe[node]);
    uchar *temp2 = uintToBytes(sizeNode + 4);
    insertArr(node, 6 + 4 * numch, temp2, 4);
    delete[] temp2;
    uchar *z8 = new uchar[8]();
    insertArr(node, sizeNode, z8, 8);
    delete[] z8;
    for (int i = 0; i < numch; i++)
    {
        changeInt(CoRe[node], 6 + 4 * i, charTouint(CoRe[node] + 6 + 4 * i) + 4);
    }
    changeInt(CoRe[node], 0, sizeNode + 12);
}
void addCh2(uint node)
{
    ushort numch = numCh(node);
    changeShort(CoRe[node], 4, numch + 2);
    uint sizeNode = 4 + charTouint(CoRe[node]);
    uchar *temp2 = uintToBytes2(sizeNode + 8, sizeNode + 16);
    insertArr(node, 6 + 4 * numch, temp2, 8);
    delete[] temp2;
    uchar *z16 = new uchar[16]();
    insertArr(node, sizeNode, z16, 16);
    delete[] z16;
    for (int i = 0; i < numch; i++)
    {
        changeInt(CoRe[node], 6 + 4 * i, charTouint(CoRe[node] + 6 + 4 * i) + 8);
    }
    changeInt(CoRe[node], 0, sizeNode + 24);
}
bool isZ8(uchar *arr)
{
    // arr이 가리키는 메모리를 uint64_t로 해석하여 비교
    return *reinterpret_cast<uint64_t *>(arr) == 0;
}
void Brain(uint node, uchar *data) // 사이즈 정보 포함해서 받아야 함
{
    auto start = std::chrono::high_resolution_clock::now();
    uint coord = 41155;
    ushort ch = 0;
    uint beforeCd = node;
    ushort beforeCh = 0;
    vector<pair<uint, ushort>> save;
    save.push_back(make_pair(beforeCd, beforeCh));
    int recyle = 0;
    uchar addCh3 = 0;
    uchar addToken = 0;
    // 첫 번째 char 확인, 없으면 추가
    uint dataSz = charTouint(data);
    uint posi = 4;
    while (posi < 4 + dataSz) // data 빌 때까지 반복
    {
        bool check2 = false;
        bool checkRecycle = false;
        coord = firstToken(data + posi, dataSz);
        // 기존 글자 비존재 하는 경우 채널 돌면서 추가 글자 확인
        uchar *wordCoord = word(coord);
        for (int i = 1; i < numCh(coord); i++)
        {
            if (beforeCd == coord && beforeCh == i)
                continue;
            if (isZ8(chVec(coord, i)))
            {
                ch = i;
                check2 = true;
                checkRecycle = true;
                continue;
            }
            uint startCoo = startCh(coord, i);
            pair<uint, ushort> dh = charToPair(CoRe[coord] + startCoo + 4);
            if (sizeRev(dh.first, 0) == 0)
            {
                continue;
            }
            else
            {
                uchar *word_dh = word(dh.first);
                uchar *strsum = combineArr(wordCoord, word_dh);
                delete[] word_dh;
                uint sizeStrsum = charTouint(strsum);
                // if (equal(strsum + 4, strsum + 4 + sizeStrsum, data + posi) && sizeStrsum <= (dataSz - posi))
                if (areEqual(strsum + 4, data + posi, sizeStrsum))
                {
                    uint coord3 = CoRe.size(); // coord3 = strsum 들어갈 좌표
                    uchar *newCh = new uchar[18];
                    for (int i = 0; i < 18; ++i)
                    {
                        newCh[i] = initValues[i];
                    }
                    CoRe.push_back(newCh);
                    addCh2(coord3);
                    pushRev2(coord3, 0, pairToBytes2(coord, 0, dh.first, 0));
                    pushCoo(coord, 0, pairToBytes(coord3, 0));
                    int szC = sizeCoo(coord, 0) / 6 - 1;

                    for (int j = szC - 1; j >= 0; j--)
                    {
                        uint startCoo2 = startCh(coord, 0);
                        uint i46j = charTouint(CoRe[coord] + startCoo2 + 4 + 6 * j);
                        uchar *wordi46j = word(i46j);
                        uint sizeWordi46j = charTouint(wordi46j);
                        if (equal(strsum + 4, strsum + 4 + sizeStrsum, wordi46j + 4) && sizeStrsum < sizeWordi46j)
                        {
                            eraseCoo(coord, 0, j);
                            pushCoo(coord3, 0, pairToBytes(i46j, 0));
                            Log(L"\"" + charToWstring(wordi46j) + L"\"을(를) \"" + charToWstring(strsum) + L"\" 하위로 이동!");
                        }
                        delete[] wordi46j;
                    }
                    pair<uint, ushort> temp;
                    pair<uint, ushort> temp2 = dh;
                    bool check3 = false;
                    int ii = 0;
                    while (dh != make_pair((uint)coord, (unsigned short)i))
                    {
                        if (++ii > 1000 || isZ8(chVec(dh.first, dh.second)))
                        {
                            check3 = true;
                            break;
                        }
                        temp = dh;
                        uint startDh = startCh(dh.first, dh.second);
                        dh = charToPair(CoRe[dh.first] + startDh + 4);
                        if (dh.first == beforeCd && dh.second == beforeCh)
                        {
                            for (int j = 1; j < save.size(); j++)
                            {
                                if (save[j] == make_pair((uint)coord, (ushort)i))
                                {
                                    temp = save[j - 1];
                                    temp2 = save[j + 1];
                                    save[j] = make_pair((uint)coord3, (ushort)1);
                                    save.erase(save.begin() + j + 1);
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    if (check3 == false)
                    {
                        clearCh(coord, i);
                        clearCh(temp.first, temp.second);
                        uint startTemp2 = startCh(temp2.first, temp2.second);
                        pair<uint, ushort> temp3 = charToPair(CoRe[temp2.first] + startTemp2 + 4);
                        clearCh(temp2.first, temp2.second); // temp2를 위의 두 줄 코드보다 먼저 clear하면 안 됨!
                        pushCoo(temp.first, temp.second, pairToBytes(coord3, 1));
                        pushCoo(coord3, 1, pairToBytes(temp3.first, temp3.second));
                        coord = coord3;
                        ch = 2;
                        check2 = true;
                        checkRecycle = false;
                        addToken++;
                        break;
                    }
                    else
                    {
                        clearCh(coord, i);
                        coord = coord3;
                        ch = 2;
                        check2 = true;
                        checkRecycle = false;
                        addToken++;
                        break;
                    }
                }
                delete[] strsum;
            }
        }
        if (checkRecycle == true)
            recyle++;
        else if (check2 == false)
        {
            ch = numCh(coord);
            addCh(coord);
            addCh3++;
        }
        wordCoord = word(coord);
        // vector<uchar> temp(data.begin() + charTouint(wordCoord2, 0), data.end());
        // data = temp;
        posi += charTouint(wordCoord);
        delete[] wordCoord;
        pushCoo(beforeCd, beforeCh, pairToBytes(coord, ch));
        beforeCd = coord;
        beforeCh = ch; // 현재 채널을 before로 이동
        save.push_back(make_pair(beforeCd, beforeCh));
    }
    pushCoo(beforeCd, beforeCh, pairToBytes(node, 0));
    Log(intToWString(recyle) + L"개 재활용!");
    Log(intToWString(addCh3) + L"개 채널 추가!");
    Log(intToWString(addToken) + L"개 Token 추가!");
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    Log(L"걸린 시간: " + intToWString(duration.count()) + L"ms");
}
std::wstring getCurrentTime()
{
    std::time_t rawtime;
    std::tm *timeinfo;
    wchar_t buffer[80];

    std::time(&rawtime);
    timeinfo = std::localtime(&rawtime);

    std::wcsftime(buffer, 80, L"%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}
wstring timeW(time_t timer)
{
    std::tm *timeinfo;
    wchar_t buffer[80];
    timeinfo = std::localtime(&timer);
    std::wcsftime(buffer, 80, L"%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}
wstring findAndUpdateOrder(uint node, ushort ch, ushort num)
{
    wstring re = L"";
    for (int i = 0; i < order[num].size(); i++)
    {
        if (node == get<0>(order[num][i]) && ch == get<1>(order[num][i]))
        {
            re = timeW(get<2>(order[num][i]));
            order[num].erase(order[num].begin() + i);
        }
    }
    time_t timer;
    time(&timer);
    order[num].push_back(make_tuple(node, ch, timer));
    return re;
}
void eraseOrder(uint node, ushort ch, ushort num)
{
    for (int i = 0; i < order[num].size(); i++)
    {
        if (node == get<0>(order[num][i]) && ch == get<1>(order[num][i]))
        {
            order[num].erase(order[num].begin() + i);
        }
    }
}
wstring last = L"";
bool isUpdateDisplay = true;
void renderLines(const std::wstring &text, short &clines, int baseX, int baseY, int offset = 0)
{
    std::vector<std::wstring> lines = splitWstring(text, L"\n");
    for (int i = 0; i < lines.size(); ++i)
    {
        std::vector<wstring> lines2 = splitLinesW(lines[i]);
        for (const auto &line2 : lines2)
        {
            string str = wstringToUtf8(line2);
            RenderText(str, baseX, baseY - (40 + offset + 15 * clines++));
        }
    }
}
void display(uint node, ushort ch)
{
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    uchar *sheetN = Sheet(node);
    wstring now = charToWstring(sheetN);
    delete[] sheetN;
    if (isUpdateDisplay)
    {
        last = findAndUpdateOrder(node, ch, 1);
        isUpdateDisplay = false;
    }
    wstring temp = L"node: " + intToWString(node) + L"/" + intToWString(CoRe.size()) +
                   L"  ch: " + intToWString(ch) + L"/" + intToWString(numCh(node) - 1) +
                   L"  Last: " + last;
    wstring temp2 = L"";
    string str = wstringToUtf8(temp);
    RenderText(str, 10, 1000 - 10);
    temp.clear();
    for (int i = 0; i < sizeRev / 6; i++)
    {
        std::wstring index = intToWString(-(i + 1)) + L". ";
        uchar *sheet = Sheet(charTouint(CoRe[node] + startRev + 4 + 6 * i));
        temp += index + charToWstring(sheet) + ((i == sizeRev / 6 - 1) ? L"" : L"\n");
        delete[] sheet;
        uint node2 = charTouint(CoRe[node] + startRev + 4 + 6 * i);
        ushort ch2 = charToushort(CoRe[node] + startRev + 4 + 6 * i + 4);
        uint startCoo2 = charTouint(CoRe[node2] + 6 + 4 * ch2);
        uint sizeCoo2 = charTouint(CoRe[node2] + startCoo2);
        uint startRev2 = startCoo2 + 4 + sizeCoo2;
        uint sizeRev2 = charTouint(CoRe[node2] + startRev2);
        for (int j = 0; j < sizeRev2 / 6; j++)
        {
            uchar *sheet2 = Sheet(charTouint(CoRe[node2] + startRev2 + 4 + 6 * j));
            temp2 += intToWString(-(i + 1)) + L"." + intToWString(j + 1) + L". " + charToWstring(sheet2) +
                     ((i == sizeRev / 6 - 1 && j == sizeRev2 / 6 - 1) ? L"" : L"\n");
            delete[] sheet2;
        }
    }
    short clines = 0;
    renderLines(temp2, clines, 40, 1000);
    renderLines(temp, clines, 50, 1000, 5);
    renderLines(now, clines, 60, 1000, 10);
    temp = L"";
    for (int i = 0; i < sizeCoo / 6; i++)
    {
        std::wstring index = intToWString(i + 1) + L". ";
        uchar *sheet = Sheet(charTouint(CoRe[node] + startCoo + 4 + 6 * i));
        temp += index + charToWstring(sheet) + L"\n";
        delete[] sheet;
    }
    renderLines(temp, clines, 70, 1000, 15);
}
void read_order(const std::string &file_path)
{
    ifstream file(file_path, ios::binary);
    while (file)
    {
        unsigned int size;
        file.read(reinterpret_cast<char *>(&size), sizeof(size));
        if (!file)
            break;
        vector<tuple<int, ushort, long long>> inner_v(size);
        for (auto &t : inner_v)
        {
            int i;
            ushort us;
            long long tt;
            file.read(reinterpret_cast<char *>(&i), sizeof(i));
            file.read(reinterpret_cast<char *>(&us), sizeof(us));
            file.read(reinterpret_cast<char *>(&tt), sizeof(tt));
            t = make_tuple(i, us, tt);
        }
        order.push_back(inner_v);
    }
    file.close();
}
void move(int numTo, uint user)
{
    uint temp{};
    uint tN = cNode[user];
    uint startCoo = startCh(cNode[user], cCh[user]);
    if (numTo > 0)
    {
        temp = numTo - 1;
        cNode[user] = charTouint(CoRe[cNode[user]] + startCoo + 4 + 6 * temp);
        cCh[user] = charToushort(CoRe[tN] + startCoo + 8 + 6 * temp); // cNode[user]를 변경했으므로 tN을 써야 함!!
    }
    else
    {
        temp = (-1 * numTo) - 1;
        uint szC = charTouint(CoRe[cNode[user]] + startCoo);
        cNode[user] = charTouint(CoRe[cNode[user]] + startCoo + 8 + szC + 6 * temp);
        cCh[user] = charToushort(CoRe[tN] + startCoo + 12 + szC + 6 * temp);
    }
}
void serializeBitmapGlyphData(std::ostream &out, const BitmapGlyphData &data)
{
    // Write bitmap size
    uint32_t bitmapSize = data.width * data.height;
    out.write(reinterpret_cast<const char *>(&bitmapSize), sizeof(bitmapSize));

    // Write bitmap data
    out.write(reinterpret_cast<const char *>(data.bitmap), bitmapSize);

    // Write other data
    out.write(reinterpret_cast<const char *>(&data.width), sizeof(data.width));
    out.write(reinterpret_cast<const char *>(&data.height), sizeof(data.height));
    out.write(reinterpret_cast<const char *>(&data.bearing), sizeof(data.bearing));
    out.write(reinterpret_cast<const char *>(&data.advance), sizeof(data.advance));
}
void deserializeBitmapGlyphData(std::istream &in, BitmapGlyphData &data)
{
    // Read bitmap size
    uint32_t bitmapSize;
    in.read(reinterpret_cast<char *>(&bitmapSize), sizeof(bitmapSize));

    // Allocate memory for bitmap and read bitmap data
    data.bitmap = new unsigned char[bitmapSize];
    in.read(reinterpret_cast<char *>(data.bitmap), bitmapSize);

    // Read other data
    in.read(reinterpret_cast<char *>(&data.width), sizeof(data.width));
    in.read(reinterpret_cast<char *>(&data.height), sizeof(data.height));
    in.read(reinterpret_cast<char *>(&data.bearing), sizeof(data.bearing));
    in.read(reinterpret_cast<char *>(&data.advance), sizeof(data.advance));
}
void saveMapToFile(const std::string &filename, const std::map<uint32_t, BitmapGlyphData> &map)
{
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Unable to open file for writing");
    }

    for (const auto &pair : map)
    {
        // Write key
        outFile.write(reinterpret_cast<const char *>(&pair.first), sizeof(pair.first));

        // Serialize and write value
        serializeBitmapGlyphData(outFile, pair.second);
    }

    outFile.close();
}
void loadMapFromFile(const std::string &filename, std::map<uint32_t, BitmapGlyphData> &map)
{
    std::ifstream inFile(filename, std::ios::binary);
    if (!inFile.is_open())
    {
        throw std::runtime_error("Unable to open file for reading");
    }

    while (!inFile.eof())
    {
        uint32_t key;
        BitmapGlyphData data;

        // Read key
        inFile.read(reinterpret_cast<char *>(&key), sizeof(key));
        if (inFile.eof())
            break;

        // Deserialize and read value
        deserializeBitmapGlyphData(inFile, data);

        map[key] = data;
    }

    inFile.close();
}
void save(string directory)
{
    auto start = std::chrono::high_resolution_clock::now();

    ofstream out(directory + "Brain3.bin", ios::binary);
    int ii = 0;
    uint size = CoRe.size();
    out.write(reinterpret_cast<const char *>(&size), 4);
    for (int i = 0; i < CoRe.size(); i++)
    {
        uint size = charTouint(CoRe[i]);
        if (size > 100000)
        {
            break;
        }
        out.write(reinterpret_cast<const char *>(CoRe[i]), size + 4);
    }
    out.close();
    ofstream file2(directory + "order2.bin", ios::binary);
    for (const auto &inner_v : order)
    {
        unsigned int size = inner_v.size();
        file2.write(reinterpret_cast<const char *>(&size), sizeof(size));
        for (const auto &t : inner_v)
        {
            file2.write(reinterpret_cast<const char *>(&get<0>(t)), sizeof(get<0>(t)));
            file2.write(reinterpret_cast<const char *>(&get<1>(t)), sizeof(get<1>(t)));
            file2.write(reinterpret_cast<const char *>(&get<2>(t)), sizeof(get<2>(t)));
        }
    }
    file2.close();
    saveMapToFile(directory + "bitmapGlyphMap.bin", bitmapGlyphMap);
    auto end = std::chrono::high_resolution_clock::now();

    // 경과 시간 계산 (밀리초 단위)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 경과 시간 출력 (밀리초 단위)
    Log(L"저장시간: " + intToWString(duration.count()) + L"ms");
}
void clearToken(uint node)
{
    uint startCoo = startCh(node, 0);
    pair<uint, ushort> cod = charToPair(CoRe[node] + startCoo + 4);
    while (cod != make_pair((uint)node, (ushort)0))
    {
        pair<uint, ushort> temp = cod;
        startCoo = startCh(cod.first, cod.second);
        cod = charToPair(CoRe[cod.first] + startCoo + 4);
        // CoRe[temp.first][temp.second] = zero8;
        clearCh(temp.first, temp.second);
    }
}
void change_data(uint node, uchar *data)
{
    clearToken(node);
    clearCh(node, 0);
    Brain(node, data);
}
void AddCon(uint add, uint coord, ushort Ch, uchar *str)
{
    if (add == CoRe.size())
    {
        uchar *newCh = new uchar[18];
        for (int i = 0; i < 18; ++i)
        {
            newCh[i] = initValues[i];
        }
        CoRe.push_back(newCh);
        addCh(CoRe.size());
    }
    else
    {
        change_data(add, str);
    }
    link(coord, Ch, add, 1);
}
void sendMsg(int ClientSocket, std::wstring content)
{
    // Create a wide string converter
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    // Convert the wide string to a multibyte string
    std::string strContent = converter.to_bytes(content);

    // Create the response
    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + std::to_string(strContent.size()) + "\r\n\r\n" + strContent;

    // Send the response
    const char *buf = response.c_str();
    int total_sent = 0;
    while (total_sent < response.size())
    {
        int sent = send(ClientSocket, buf + total_sent, response.size() - total_sent, 0);
        if (sent == -1)
        {
            // 오류 처리
            std::cerr << "send failed with error: " << std::endl;
            break;
        }
        total_sent += sent;
    }
}
wstring contentList(uint node, ushort ch)
{
    wstring ws = L"";
    uint startCoo = charTouint(CoRe[node] + 6 + 4 * ch);
    uint sizeCoo = charTouint(CoRe[node] + startCoo);
    uint startRev = startCoo + 4 + sizeCoo;
    uint sizeRev = charTouint(CoRe[node] + startRev);
    if (sizeRev > 0)
    {
        uchar *prevNode = axis2(node, ch);
        wstring prev = L"";
        wstring pprev = L"";
        for (int i = 0; i < sizeRev / 6; i++)
        {
            wstring hpL = L"<a href='javascript:__doPostBack(\"LinkButtonRev" + intToWString(i + 1) + L"\", \"\")' onclick='document.getElementById(\"textinput\").value =" + intToWString(-(i + 1)) + L"; sendTextToServer(); return false;'>" + intToWString(-(i + 1)) + L"</a>";
            prev += hpL + L". " + charToWstring(Sheet(charTouint(prevNode + 6 * i))) + L"<br/>";
            // prev += hpL + L". " + charToWstring(Sheet(*reinterpret_cast<uint *>(&prevNode[6 * i]))) + L"<br/>";
            uint node2 = charTouint(prevNode + 6 * i);
            ushort ch2 = charToushort(prevNode + 4 + 6 * i);
            uint startCoo2 = charTouint(CoRe[node2] + 6 + 4 * ch2);
            uint sizeCoo2 = charTouint(CoRe[node2] + startCoo2);
            uint startRev2 = startCoo2 + 4 + sizeCoo2;
            uint sizeRev2 = charTouint(CoRe[node2] + startRev2);
            uchar *splPrev2 = CoRe[node2] + startRev2 + 4 + 6 * i;
            // vector<uchar> splPrev2 = axon2(charTouint(prevNode, 6 * i), charToushort(prevNode, 4 + 6 * i));
            for (int j = 0; j < sizeRev2 / 6; j++)
            {
                // pprev += intToWString(-(i + 1)) + L"." + intToWString(j + 1) + L" " + charToWstring(Sheet(*reinterpret_cast<uint *>(&splPrev2[6 * j]))) + L"<br/>";
                pprev += intToWString(-(i + 1)) + L"." + intToWString(j + 1) + L" " + charToWstring(Sheet(charTouint(splPrev2 + 6 * j))) + L"<br/>";
            }
        }
        ws += pprev + L"<p class='prev'>" + prev;
    }
    ws += L"<p class='this'>" + charToWstring(Sheet(node)) + L"<br/>" + L"<p class='next'>"; // 현재 단계

    uchar *nextNode = CoRe[node] + startCoo + 4;
    uint page = 1;
    for (int i = (page - 1) * 50; i < page * 50 - 1 && sizeCoo != 0; i++)
    {
        if (i == sizeCoo / 6)
        {
            break;
        }
        wstring hpL = L"<a href='javascript:__doPostBack(\"LinkButtonNext" + intToWString(i + 1) + L"\", \"\")' onclick='document.getElementById(\"textinput\").value =" + intToWString(i + 1) + L"; sendTextToServer(); return false;'>" + intToWString(i + 1) + L"</a>";
        // ws += hpL + L". " + charToWstring(Sheet(*reinterpret_cast<uint *>(&nextNode[6 * i]))) + L"<br/>";
        ws += hpL + L". " + charToWstring(Sheet(charTouint(nextNode + 6 * i))) + L"<br/>";
    }
    if (sizeCoo / 6 > 50)
    {
        ws += L"<br />Page ";
        for (int i = 1; i <= (sizeCoo / 6 / 50) + 1; i++)
        {
            ws += L" <a href='javascript:__doPostBack(\"PageButton" + intToWString(i) + L"\", \"\")' onclick='document.getElementById(\"textinput\").value =\"Page\" + " + intToWString(i) + L"; sendTextToServer(); return false;'>" + intToWString(i) + L"</a>";
        }
    }
    ws += L"<style> .prev{margin-left: 10px;} .this{margin-left: 20px;} .next{margin-left:30px;}</style>";
    return ws;
}
void study(uint user)
{
    uint t1 = get<0>(order[user][0]);
    ushort t2 = get<1>(order[user][0]);
    while (numCh(t1) > t2 + 1)
    {
        order[user].erase(order[user].begin());
        t1 = get<0>(order[user][0]);
        t2 = get<1>(order[user][0]);
    }
    cNode[user] = t1;
    cCh[user] = t2;
    tuple<int, ushort, time_t> temp = order[user][0];
    eraseOrder(t1, t2, user);
    order[user].push_back(temp);
}
int Network()
{
    struct sockaddr_in server_addr;
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Server socket creation failed.\n";
        return -1;
    }
    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8000); // 포트 번호는 8000으로 설정
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
    {
        std::cerr << "setsockopt error.\n";
        return -1;
    }
    // 서버 소켓에 주소 바인드
    if (bind(serverSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cerr << "Bind error.\n";
        return -1;
    }
    // 클라이언트의 연결 요청을 대기
    if (listen(serverSocket, 5) == -1)
    {
        std::cerr << "Listen error.\n";
        return -1;
    }
    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_size = sizeof(client_addr);
        int client_socket = accept(serverSocket, (struct sockaddr *)&client_addr, &client_addr_size);

        if (client_socket == -1)
        {
            std::cerr << "Accept error.\n";
            continue;
        }

        // 클라이언트와의 통신을 위한 코드를 여기에 작성하세요.
        char buffer[1024];
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer) - 1, 0); // Leave space for null terminator
        if (bytesReceived > 0)
        {
            buffer[bytesReceived] = '\0'; // Null-terminate the received data
                                          // Create a wide string converter
            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

            // Convert the multibyte string to a wide string
            std::wstring ws = converter.from_bytes(buffer);
            // wchar_t type3[1024];
            // int charsConverted = MultiByteToWideChar(CP_UTF8, 0, buffer, bytesReceived, type3, sizeof(type3) / sizeof(wchar_t));
            wstring firstLine = ws.substr(0, ws.find(L"\r\n"));
            wstring method = firstLine.substr(0, firstLine.find(L" "));
            if (method == L"GET")
            {
                std::ifstream file("/home/yonghwan/test/Save_and_BackUp/index.html");
                if (!file.is_open())
                {
                    std::cerr << "Failed to open file" << std::endl;
                    continue; // Skip this iteration
                }
                std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
                std::string response = "HTTP/1.1 200 OK\r\nContent-Length: " + to_string(content.size()) + "\r\n\r\n" + content;
                send(client_socket, response.c_str(), response.size(), 0);
            }
            // else if (method == L"POST")

            // {
            //     std::wstring prefix = L"\r\n\r\n";
            //     size_t startPos = ws.find(prefix);
            //     if (startPos != std::wstring::npos)
            //     {
            //         wstring clientMessage = ws.substr(startPos + 4);
            //         std::vector<std::wstring> clientMvec = splitWstring(clientMessage, L"\t");
            //         wstringstream wss(clientMvec[0]);
            //         uint user{};
            //         wss >> user;
            //         wstring content = L"";
            //         if (user == 0)
            //         { // 처음 접속 시 보내는 내용(로그인 화면)
            //             if (clientMvec[1] == L"0")
            //             {
            //                 uchar *sheet34198 = Sheet(34198);
            //                 content = intToWString(user) + L"\t" + intToWString(34198) + L"\t" + L"0" + L"\t" + charToWstring(sheet34198 + 4, charTouint(sheet34198, 0)); // 아이디 입력 화면을 보냄 user, node, ch, sheet 순서
            //                 delete[] sheet34198;
            //                 sendMsg(client_socket, content);
            //             }
            //             else if (clientMvec[1] == L"34198")
            //             { // 아이디를 입력한 상태(비밀번호 입력 화면을 보내야 함)
            //                 // vector<uchar> IDList = CoRe[34196][1].first;
            //                 uchar *IDList = axis1(34196, 1);
            //                 bool check = false;
            //                 for (int i = 0; i < IDList.size(); i++)
            //                 {
            //                     if (clientMvec[3] == charToWstring(Sheet(*reinterpret_cast<uint *>(&IDList[6 * i])))) // ID가 존재하는 경우
            //                     {
            //                         check = true;
            //                         content = intToWString(i + 1) + L"\t" + intToWString(34199) + L"\t" + L"0" + L"\t" + charToWstring(Sheet(34199)); // password 입력 화면을 보냄 user, node, sheet 순서
            //                         sendMsg(client_socket, content);
            //                         break;
            //                     }
            //                 }
            //                 if (!check)
            //                 {
            //                     content = intToWString(user) + L"\t" + intToWString(34198) + L"\t" + L"0" + L"\t" + L"없는 아이디입니다. 다시 아이디를 입력해 주세요."; // 다시 아이디 입력 화면을 보냄
            //                     sendMsg(client_socket, content);
            //                 }
            //             }
            //         }
            //         else
            //         { // 아이디까지 입력한 이후 상태
            //             if (clientMvec[1] == L"34199")
            //             { // 비밀번호 입력한 상태
            //                 uint startCoo = startCh(34196, 1);
            //                 pair<uint, ushort> userID = charToPair(CoRe[34196], startCoo + 4 + 6 * (user - 1));
            //                 uint startUserID = startCh(userID.first, userID.second);
            //                 pair<uint, ushort> Pass = charToPair(CoRe[userID.first], startUserID + 4);
            //                 if (clientMvec[3] == charToWstring(Sheet(Pass.first)))
            //                 {
            //                     uint startPass = startCh(Pass.first, Pass.second);
            //                     pair<uint, ushort> start = charToPair(CoRe[Pass.first], startPass + 4);
            //                     content = intToWString(user) + L"\t" + intToWString(start.first) + L"\t" + intToWString(start.first) + L"\t" + contentList(start.first, start.second); // 시작 화면을 보냄
            //                     sendMsg(client_socket, content);
            //                 }
            //                 else
            //                 {
            //                     content = intToWString(user) + L"\t" + intToWString(34199) + L"\t" + L"0" + L"\t" + L"비밀번호가 틀립니다. 다시 입력해 주세요."; // password 입력 화면을 보냄 user, node, ch, sheet 순서
            //                     sendMsg(client_socket, content);
            //                 }
            //             }
            //             else
            //             { // LogIn 이후 상태
            //                 int num = 0;
            //                 try
            //                 {
            //                     num = stoi(clientMvec[3]);
            //                     if (num == 98)
            //                     {
            //                         study(user);
            //                         content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cNode[user], cCh[user]);
            //                         sendMsg(client_socket, content);
            //                     }
            //                     else if ((num > 0 && num <= sizeCoo(cNode[user], cCh[user]) / 6) || (num < 0 && -num <= sizeRev(cNode[user], cCh[user]) / 6))
            //                     {
            //                         move(num, user);
            //                         content = intToWString(user) + L"\t" + intToWString(cNode[user]) + L"\t" + intToWString(cCh[user]) + L"\t" + contentList(cNode[user], cCh[user]);
            //                         sendMsg(client_socket, content);
            //                     }
            //                 }
            //                 catch (std::invalid_argument &e)
            //                 {
            //                     std::cout << "Invalid argument: the wstring cannot be converted to an integer." << std::endl;
            //                 }
            //                 catch (std::out_of_range &e)
            //                 {
            //                     std::cout << "Out of range: the wstring is too large to be converted to an integer." << std::endl;
            //                 }
            //             }
            //         }
            //     }
            // }
        }

        close(client_socket);
    }

    close(serverSocket);
}
uint wstringToUint(const std::wstring &input)
{
    std::wistringstream wiss(input);
    uint number = 0;
    wiss >> number;
    return number;
}
unsigned int stringToUint(const std::string &input)
{
    std::istringstream iss(input);
    unsigned int number = 0;
    iss >> number;
    return number;
}
void copyFile(const std::string &srcPath, const std::string &destPath)
{
    std::ifstream src(srcPath, std::ios::binary);
    std::ofstream dest(destPath, std::ios::binary);

    if (!src)
    {
        throw std::runtime_error("Source file could not be opened.");
    }

    if (!dest)
    {
        throw std::runtime_error("Destination file could not be opened.");
    }

    dest << src.rdbuf();
}
uint sizeGarbage()
{
    int i = 0;
    uint temp = charTouint(CoRe[gar] + 14);
    while (temp != gar)
    {
        temp = charTouint(CoRe[temp] + 14);
        i++;
    }
    return i;
}
size_t vmSize = 0, vmRSS = 0;
int fps = 0;
void info()
{
    uchar *sh1 = Sheet(34875);
    uchar *sh2 = Sheet(34878);
    uchar *sh3 = Sheet(34880);
    wstring ww = L"\norder.size: " + intToWString(order[1].size() - 1) + L"\nGarbage.size: " + intToWString(sizeGarbage()) + L"\nCsave: " + charToWstring(sh1) + L"," + charToWstring(sh2) + L"," + charToWstring(sh3) + L"\nvmSize: " + intToWString(vmSize) + L" kB\nvmRSS: " + intToWString(vmRSS) + L" kB\nFPS: " + intToWString(fps);
    delete[] sh1;
    delete[] sh2;
    delete[] sh3;
    std::vector<std::wstring> lines = splitWstring(ww, L"\n");
    short ll = lines.size();
    for (int i = 0; i < ll; ++i)
    {
        string line = wstringToUtf8(lines[i]);
        RenderText(line, 1000, 1000 - (20 + 15 * i));
    }
}
void pushGarbage(uint node)
{
    uint next = charTouint(CoRe[gar] + 10);
    uchar newCh[18] = {14, 0, 0, 0, 1, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    delete[] CoRe[gar];
    CoRe[gar] = newCh;

    uchar newCh2[18] = {14, 0, 0, 0, 1, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    delete[] CoRe[node];
    CoRe[node] = newCh2;
    pushCoo(gar, 0, pairToBytes(node, 0));
    pushCoo(node, 0, pairToBytes(next, 0));
}
void deleteNode(uint node)
{
    if (sizeRev(node, 0) == 6)
    {
        clearToken(node);
    }
    uint numch = numCh(node);
    for (int i = 0; i < numch; i++)
    {
        vector<uint> sizeCRn = sizeCoRe(node, i);
        uint startI = startCh(node, i);
        for (int j = 0; j < sizeCRn[0] / 6; j++)
        {
            uint startI = startCh(node, i);
            pair<uint, ushort> temp = charToPair(CoRe[node] + startI + 4 + 6 * j);
            // int size = CoRe[temp.first].size();
            int size = numCh(temp.first);
            if (size > 0 && size <= temp.second + 1)
            {
                vector<uint> szCR = sizeCoRe(temp.first, temp.second);
                if (szCR[1] > 0)
                {
                    for (int k = 0; k < szCR[1] / 6; k++)
                    {
                        uint startTemp = startCh(temp.first, temp.second);
                        pair<uint, ushort> temp2 = charToPair(CoRe[temp.first] + startTemp + 8 + szCR[0] + 6 * k);
                        if (temp2 == make_pair((uint)node, (ushort)i))
                        {
                            // eraseRev(CoRe[temp.first], startTemp, 8 + szCR[0] + 6 * k);
                            eraseRev(temp.first, temp.second, k);
                            szCR[1] = szCR[1] - 6;
                        }
                    }
                }
            }
        }
        for (int j = 0; j < sizeCRn[1] / 6; j++)
        {
            pair<uint, ushort> temp = charToPair(CoRe[node] + startI + 8 + sizeCRn[0] + 6 * j);
            int size = numCh(temp.first);
            if (size > 0 && size <= temp.second + 1)
            {
                uint startTemp = startCh(temp.first, temp.second);
                uint szC = charTouint(CoRe[temp.first] + startTemp);
                if (szC > 0)
                {
                    for (int k = 0; k < szC / 6; k++)
                    {
                        pair<uint, ushort> temp2 = charToPair(CoRe[temp.first] + startTemp + 4 + 6 * k);
                        if (temp2 == make_pair((uint)node, (ushort)i))
                        {
                            // eraseCoo(CoRe[temp.first], startTemp, 4 + 6 * k);
                            eraseCoo(temp.first, temp.second, k);
                            szC = szC - 6; // szC를 6 빼주어야 다음 loop에서 오류 안 남!!
                        }
                    }
                }
            }
        }
        eraseOrder(node, i, 1);
    }
    // CoRe[node].clear();
    pushGarbage(node);
}
void cut(uint node, ushort ch, uint index)
{
    vector<uint> sizeCRn = sizeCoRe(node, ch);
    uint startCoo = startCh(node, ch);
    pair<uint, ushort> temp = charToPair(CoRe[node] + startCoo + 4 + 6 * index);
    uint startTemp = startCh(temp.first, temp.second);
    int size = numCh(temp.first);
    if (size > 0 && size <= temp.second + 1)
    {
        vector<uint> szCR = sizeCoRe(temp.first, temp.second);
        if (szCR[1] > 0)
        {
            for (int k = 0; k < szCR[1] / 6; k++)
            {
                if (temp == make_pair(cNode[1], cCh[1]))
                {
                    // eraseRev(CoRe[temp.first], startTemp, 8 + szCR[0] + 6 * k);
                    eraseRev(temp.first, temp.second, k);
                }
            }
        }
    }
    // eraseCoo(CoRe[node], startCoo, 4 + 6 * index);
    eraseCoo(node, ch, index);
}
uint popGarbage()
{
    int i = 0;
    uint re = charTouint(CoRe[gar] + 14);
    uint next = charTouint(CoRe[re] + 14);
    clearCh(re, 0);
    clearCh(gar, 0);
    pushCoo(gar, 0, pairToBytes(next, 0));
    return re;
}
uint checkRing()
{
    uint re = 0;
    int dd = 0;
    int jj = 0;
    for (int i = 0; i < CoRe.size(); i++)
    {
        uint rev = sizeRev(i, 0);
        if (rev == 0)
            continue;
        else
        {
            dd++;
            uint numch = numCh(i);
            for (int j = 1; j < numch; j++)
            {
                uint startJ = startCh(i, j);
                if (isZ8(chVec(i, j)))
                    continue;
                else
                {
                    pair<uint, ushort> dh = charToPair(CoRe[i] + startJ + 4);
                    int ii = 0;
                    while (dh != make_pair((uint)i, (ushort)j))
                    {
                        if (isZ8(chVec(dh.first, dh.second)))
                        {
                            clearCh(i, j);
                            jj++;
                            break;
                        }
                        uint startDh = startCh(dh.first, dh.second);
                        dh = charToPair(CoRe[dh.first] + startDh + 4);
                        ii++;
                        if (ii == 1000)
                        {
                            clearCh(i, j);
                            re++;
                            break;
                        }
                    }
                }
            }
        }
    }
    return re;
}
// 전역 변수로 문자열을 저장
std::string inputText = "";
vector<string> linesOfEditControl;
size_t cursorRow = 0;
size_t cursorCol = 0;
void clearInputText()
{
    inputText.clear();
    linesOfEditControl = splitLines(inputText);
    cursorRow = 0;
    cursorCol = 0;
}
bool tryConvertToInt(const std::string &str, int &outValue)
{
    std::istringstream iss(str);
    iss >> outValue;

    // 파싱 후 스트림의 끝에 도달했는지 확인 (다른 문자가 없는지 확인)
    return !iss.fail() && iss.eof();
}
bool getMemoryUsage(size_t &vmSize, size_t &vmRSS)
{
    std::ifstream statusFile("/proc/self/status");
    std::string line;

    if (!statusFile.is_open())
    {
        return false;
    }

    while (std::getline(statusFile, line))
    {
        if (line.substr(0, 6) == "VmSize")
        {
            sscanf(line.c_str(), "VmSize: %lu kB", &vmSize);
        }
        else if (line.substr(0, 6) == "VmRSS:")
        {
            sscanf(line.c_str(), "VmRSS: %lu kB", &vmRSS);
        }
    }

    statusFile.close();
    return true;
}
#include <iomanip>
std::string bitmapToHexString(unsigned char *bitmap, uint size)
{
    std::stringstream ss;
    for (uint i = 0; i < size; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bitmap[i]);
        if (i < size - 1)
            ss << " ";
    }
    return ss.str();
}
uchar *hexStringToBitmap(const std::string &hexString)
{
    std::istringstream ss(hexString);
    std::vector<unsigned char> bytes;

    std::string hexByte;
    while (ss >> hexByte)
    {
        unsigned int byteValue;
        std::stringstream hexStream(hexByte);
        hexStream >> std::hex >> byteValue;
        bytes.push_back(static_cast<unsigned char>(byteValue));
    }

    unsigned char *bitmap = new unsigned char[bytes.size()];
    std::copy(bytes.begin(), bytes.end(), bitmap);

    return bitmap;
}
std::string codepointToUtf8(unsigned int cp)
{
    std::string result;
    if (cp <= 0x7F)
    {
        result.push_back(cp);
    }
    else if (cp <= 0x7FF)
    {
        result.push_back((cp >> 6) | 192);
        result.push_back((cp & 63) | 128);
    }
    else if (cp <= 0xFFFF)
    {
        result.push_back((cp >> 12) | 224);
        result.push_back(((cp >> 6) & 63) | 128);
        result.push_back((cp & 63) | 128);
    }
    else if (cp <= 0x10FFFF)
    {
        result.push_back((cp >> 18) | 240);
        result.push_back(((cp >> 12) & 63) | 128);
        result.push_back(((cp >> 6) & 63) | 128);
        result.push_back((cp & 63) | 128);
    }
    return result;
}
std::string printBitmapGlyphData(uint32_t unicodePoint)
{
    std::stringstream output;

    auto it = bitmapGlyphMap.find(unicodePoint);
    if (it != bitmapGlyphMap.end())
    {
        BitmapGlyphData &data = it->second;

        output << "Bitmap Glyph Data:" << codepointToUtf8(unicodePoint) << std::endl;
        output << "Width: " << data.width << std::endl;
        output << "Height: " << data.height << std::endl;
        output << "Bearing: (" << data.bearing.x << ", " << data.bearing.y << ")" << std::endl;
        output << "Advance: " << data.advance << std::endl;
        output << "Bitmap Data: " << bitmapToHexString(data.bitmap, data.width * data.height) << std::endl;
    }
    else
    {
        output << "No glyph data found for unicode point: " << unicodePoint << std::endl;
    }

    return output.str();
}
void modifyGlyphData(uint32_t unicodePoint, const BitmapGlyphData &newData)
{
    // 해당 유니코드 포인트의 글리프 데이터를 찾습니다.
    auto it = bitmapGlyphMap.find(unicodePoint);
    if (it != bitmapGlyphMap.end())
    {
        // 글리프 데이터를 업데이트합니다.
        delete[] it->second.bitmap;
        it->second.bitmap = newData.bitmap;
        it->second.width = newData.width;
        it->second.height = newData.height;
        it->second.bearing = newData.bearing;
        it->second.advance = newData.advance;
    }
    else
    {
        // 해당 유니코드 포인트의 글리프 데이터가 존재하지 않는 경우
        // 새로운 글리프 데이터를 맵에 추가합니다.
        bitmapGlyphMap[unicodePoint] = newData;
    }
}
void AddStringToNode(const string &str, uint node, ushort ch)
{
    wstring wstr = utf8ToWstring(str);
    uchar *wstr2 = wstringToUChar(wstr);
    time_t timer;
    time(&timer);
    uint newcd = CoRe.size();
    if (sizeGarbage() > 10)
    { // 쓰레기통에서 재활용
        newcd = popGarbage();
        addCh(newcd); // make count of ch as 2
        link(node, ch, newcd, 1);
        order[1].push_back(make_tuple(newcd, 1, timer));
        Brain(newcd, wstr2);
    }
    else
    {
        uchar *newCh2 = new uchar[18];
        for (int i = 0; i < 18; ++i)
        {
            newCh2[i] = initValues[i];
        }
        CoRe.push_back(newCh2);
        addCh(newcd);
        link(node, ch, newcd, 1);
        order[1].push_back(make_tuple(newcd, 1, timer));
        Brain(newcd, wstr2);
    }
    delete[] wstr2;
}
void submit()
{
    int length = inputText.size();
    if (length == 0)
    {
        Log(L"Edit control이 비어 있습니다!");
    }
    wchar_t *buffer = new wchar_t[length + 1];
    wstring bf = wstring(buffer);
    int convertedInt;
    if (tryConvertToInt(inputText, convertedInt))
    {
        vector<uint> sizeCR = sizeCoRe(cNode[1], cCh[1]);
        if ((convertedInt > 0 && convertedInt <= sizeCR[0] / 6) ||
            (convertedInt < 0 && -convertedInt <= sizeCR[1] / 6))
        {
            move(convertedInt, 1);
            clearInputText();
            isUpdateDisplay = true;
        }
        else if (inputText == "98")
        {
            study(1);
            info();
            isUpdateDisplay = true;
        }
    }
    else if (inputText[0] == '/')
    {
        string str = inputText.substr(1);
        AddStringToNode(str, cNode[1], cCh[1]);
        display(cNode[1], cCh[1]);
        clearInputText();
        info();
    }
    else if (inputText[0] == '@')
    {
        string str = inputText.substr(1);
        wstring wstr = utf8ToWstring(str);
        uchar *wstr2 = wstringToUChar(wstr);
        change_data(cNode[1], wstr2);
        clearInputText();
        delete[] wstr2;
    }
    else if (inputText == "save" || inputText == "저장")
    {
        uchar *sheet34875 = Sheet(34875);
        unsigned int Csave = stoi(charToWstring(sheet34875));
        delete[] sheet34875;
        uchar *sheet34878 = Sheet(34878);
        unsigned int Csave2 = stoi(charToWstring(sheet34878));
        delete[] sheet34878;
        uchar *sheet34880 = Sheet(34880);
        unsigned int Csave3 = stoi(charToWstring(sheet34880));
        delete[] sheet34880;
        wstring ww;
        // if (Csave2 >= 5)
        // {
        //     copyFile("/home/yonghwan/test/Save_and_BackUp/BackUp2/Brain3.bin", "/home/yonghwan/test/Save_and_BackUp/BackUp3/Brain3.bin");
        //     copyFile("/home/yonghwan/test/Save_and_BackUp/BackUp2/order2.bin", "/home/yonghwan/test/Save_and_BackUp/BackUp3/order2.bin");
        //     ww = intToWString(0);
        //     uchar* ww2 = wstringToUChar(ww);
        //     change_data(34878, ww2);
        //     delete[] ww2;
        //     Log(L"백업 2 완료!");
        //     ww = intToWString(Csave3 + 1);
        //     uchar* ww3 = wstringToUChar(ww);
        //     change_data(34880, ww3);
        //     delete[] ww3;
        // }
        // if (Csave >= 5)
        // {
        //     copyFile("/home/yonghwan/test/Save_and_BackUp/BackUp1/Brain3.bin", "/home/yonghwan/test/Save_and_BackUp/BackUp2/Brain3.bin");
        //     copyFile("/home/yonghwan/test/Save_and_BackUp/BackUp1/order2.bin", "/home/yonghwan/test/Save_and_BackUp/BackUp2/order2.bin");
        //     Csave = -1;
        //     ww = intToWString(Csave);
        //     uchar* ww2 = wstringToUChar(ww);
        //     change_data(34875, ww2);
        //     delete[] ww2;
        //     Log(L"백업 1 완료!");
        //     ww = intToWString(Csave2 + 1);
        //     uchar* ww3 = wstringToUChar(ww);
        //     change_data(34878, ww3);
        //     delete[] ww3;
        // }

        // copyFile("/home/yonghwan/test/Save_and_BackUp/BackUp0/Brain3.bin", "/home/yonghwan/test/Save_and_BackUp/BackUp1/Brain3.bin");
        // copyFile("/home/yonghwan/test/Save_and_BackUp/BackUp0/order2.bin", "/home/yonghwan/test/Save_and_BackUp/BackUp1/order2.bin");
        // ww = intToWString(Csave + 1);
        // uchar* ww2 = wstringToUChar(ww);
        // change_data(34875, ww2);
        // delete[] ww2;
        save("/home/yonghwan/test/Save_and_BackUp/BackUp0/");
        info();
        clearInputText();
    }
    else if (inputText == "시작")
    {
        cNode[1] = 0;
        cCh[1] = 1;
        clearInputText();
    }
    else if (inputText == "수정")
    {
        uchar *sheetNode = Sheet(cNode[1]);
        inputText = wstringToUtf8(L"@" + charToWstring(sheetNode));
        delete[] sheetNode;
        linesOfEditControl = splitLines(inputText);
        cursorRow = linesOfEditControl.size() - 1;
        cursorCol = linesOfEditControl[cursorRow].length();
    }
    else if (inputText == "del98")
    { // 삭제하고 + 98
        deleteNode(cNode[1]);
        study(1);
        display(cNode[1], cCh[1]);
        inputText = "98";
        info();
    }
    else if (inputText.size() >= 4 && inputText.substr(0, 4) == "103,")
    { // 연결 해제
        vector<string> spl = splitStringASCII(inputText, ',');
        if (spl.size() == 2)
        {
            int tt = stringToUint(spl[1]) - 1;
            cut(cNode[1], cCh[1], tt);
        }
        else
        {
            Log(L"올바른 입력 형식이 아닙니다. ");
        }
        display(cNode[1], cCh[1]);
        inputText.clear();
    }
    else if (inputText.size() >= 4 && inputText.substr(0, 4) == "104,")
    { // 자식 node로 이동
        vector<string> spl = splitStringASCII(inputText, ',');
        if (spl.size() == 3)
        {
            uint startCoo = startCh(cNode[1], cCh[1]);
            uint res2 = charTouint(CoRe[cNode[1]] + startCoo + 4 + 6 * (stringToUint(spl[1]) - 1));
            int tt = 6 * (stringToUint(spl[2]) - 1);
            CoMove(cNode[1], cCh[1], res2, charTouint(CoRe[cNode[1]] + startCoo + 4 + tt), charToushort(CoRe[cNode[1]] + startCoo + 8 + tt));
        }
        else
        {
            Log(L"올바른 입력 형식이 아닙니다. ");
        }
        clearInputText();
    }
    else if (inputText.size() >= 4 && inputText.substr(0, 4) == "del,")
    {
        vector<string> spl = splitStringASCII(inputText, ',');
        if (spl.size() == 2)
        {
            uint startCoo = startCh(cNode[1], cCh[1]);
            int tt = stringToUint(spl[1]) - 1;
            uint deln = charTouint(CoRe[cNode[1]] + startCoo + 4 + 6 * tt);
            cut(cNode[1], cCh[1], tt);
            deleteNode(deln);
            Log(intToWString(deln) + L" 삭제!");
        }
        else
        {
            Log(L"올바른 입력 형식이 아닙니다. ");
        }
        info();
        clearInputText();
    }
    else if (inputText.size() >= 5 && inputText.substr(0, 5) == "move,")
    {
        vector<string> spl = splitStringASCII(inputText, ',');
        if (spl.size() == 3)
        {
            uint node = stringToUint(spl[1]);
            ushort ch = stringToUint(spl[2]);
            cNode[1] = node;
            cCh[1] = ch;
        }
        else
        {
            Log(L"올바른 입력 형식이 아닙니다. ");
        }
        clearInputText();
    }
    else if (inputText.size() >= 8 && inputText.substr(0, 8) == "unicode,")
    {
        vector<string> spl = splitStringASCII(inputText, ',');
        if (spl.size() == 2)
        {
            uint unicode = stringToUint(spl[1]);
            wstring ws = utf8ToWstring(printBitmapGlyphData(unicode));
            Log(ws);
        }
        else if (spl.size() >= 3)
        {
            if (spl[1] == "fix")
            {
                uint unicode = stringToUint(spl[2]);
                uint width = stringToUint(spl[3]);
                uint height = stringToUint(spl[4]);
                int bearingX = stringToUint(spl[5]);
                int bearingY = stringToUint(spl[6]);
                glm::ivec2 bearing(bearingX, bearingY);
                uint advance = stringToUint(spl[7]);
                uchar *bitmap = hexStringToBitmap(spl[8]);
                BitmapGlyphData newData = {bitmap, width, height, bearing, advance};
                modifyGlyphData(unicode, newData);
            }
        }
    }
    delete[] buffer;
}
std::string currentChosung;
std::string currentJungsung;
string currentJongsung;
string currentJongsung2;
std::map<std::string, int> chosungIndex = {
    {"ㄱ", 0}, {"ㄲ", 1}, {"ㄴ", 2}, {"ㄷ", 3}, {"ㄸ", 4}, {"ㄹ", 5}, {"ㅁ", 6}, {"ㅂ", 7}, {"ㅃ", 8}, {"ㅅ", 9}, {"ㅆ", 10}, {"ㅇ", 11}, {"ㅈ", 12}, {"ㅉ", 13}, {"ㅊ", 14}, {"ㅋ", 15}, {"ㅌ", 16}, {"ㅍ", 17}, {"ㅎ", 18}};
std::map<std::string, int> jungsungIndex = {
    {"ㅏ", 0}, {"ㅐ", 1}, {"ㅑ", 2}, {"ㅒ", 3}, {"ㅓ", 4}, {"ㅔ", 5}, {"ㅕ", 6}, {"ㅖ", 7}, {"ㅗ", 8}, {"ㅘ", 9}, {"ㅙ", 10}, {"ㅚ", 11}, {"ㅛ", 12}, {"ㅜ", 13}, {"ㅝ", 14}, {"ㅞ", 15}, {"ㅟ", 16}, {"ㅠ", 17}, {"ㅡ", 18}, {"ㅢ", 19}, {"ㅣ", 20}};
std::map<std::string, int> jongsungIndex = {
    {"", 0}, {"ㄱ", 1}, {"ㄲ", 2}, {"ㄳ", 3}, {"ㄴ", 4}, {"ㄵ", 5}, {"ㄶ", 6}, {"ㄷ", 7}, {"ㄹ", 8}, {"ㄺ", 9}, {"ㄻ", 10}, {"ㄼ", 11}, {"ㄽ", 12}, {"ㄾ", 13}, {"ㄿ", 14}, {"ㅀ", 15}, {"ㅁ", 16}, {"ㅂ", 17}, {"ㅄ", 18}, {"ㅅ", 19}, {"ㅆ", 20}, {"ㅇ", 21}, {"ㅈ", 22}, {"ㅊ", 23}, {"ㅋ", 24}, {"ㅌ", 25}, {"ㅍ", 26}, {"ㅎ", 27}};
std::map<std::pair<std::string, std::string>, std::string> combinedJungsung = {
    {{"ㅡ", "ㅣ"}, "ㅢ"},
    {{"ㅗ", "ㅏ"}, "ㅘ"},
    {{"ㅗ", "ㅐ"}, "ㅙ"},
    {{"ㅗ", "ㅣ"}, "ㅚ"},
    {{"ㅜ", "ㅓ"}, "ㅝ"},
    {{"ㅜ", "ㅔ"}, "ㅞ"},
    {{"ㅜ", "ㅣ"}, "ㅟ"},
    // 필요하다면 추가 복합 모음 조합을 계속해서 여기에 추가할 수 있습니다.
};
std::map<std::pair<std::string, std::string>, std::string> combinedJongsung = {
    {{"ㄱ", "ㅅ"}, "ㄳ"},
    {{"ㄴ", "ㅈ"}, "ㄵ"},
    {{"ㄴ", "ㅎ"}, "ㄶ"},
    {{"ㄹ", "ㄱ"}, "ㄺ"},
    {{"ㄹ", "ㅁ"}, "ㄻ"},
    {{"ㄹ", "ㅂ"}, "ㄼ"},
    {{"ㄹ", "ㅅ"}, "ㄽ"},
    {{"ㄹ", "ㅌ"}, "ㄾ"},
    {{"ㄹ", "ㅍ"}, "ㄿ"},
    {{"ㄹ", "ㅎ"}, "ㅀ"},
    {{"ㅂ", "ㅅ"}, "ㅄ"},
};
std::string char16ToString(char16_t ch)
{
    std::string result;

    if (ch <= 0x7F)
    { // 1-byte UTF-8
        result.push_back(static_cast<char>(ch));
    }
    else if (ch <= 0x7FF)
    { // 2-byte UTF-8
        result.push_back(static_cast<char>(0xC0 | ((ch >> 6) & 0x1F)));
        result.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
    }
    else
    { // 3-byte UTF-8
        result.push_back(static_cast<char>(0xE0 | ((ch >> 12) & 0x0F)));
        result.push_back(static_cast<char>(0x80 | ((ch >> 6) & 0x3F)));
        result.push_back(static_cast<char>(0x80 | (ch & 0x3F)));
    }

    return result;
}
char16_t composeHangul(int chosungIdx, int jungsungIdx, int jongsungIdx)
{
    return 0xAC00 + (chosungIdx * 21 * 28) + (jungsungIdx * 28) + jongsungIdx;
}
void updateCursorPosition(size_t newCharLength)
{
    linesOfEditControl = splitLines(inputText);
    if (newCharLength == 0)
        return;

    cursorCol += newCharLength;
    if (cursorCol >= linesOfEditControl[cursorRow].length() && cursorRow < linesOfEditControl.size() - 1)
    {
        cursorCol -= linesOfEditControl[cursorRow].length();
        cursorRow++;
    }
}
void insertKey(const std::string character)
{
    // 커서의 위치를 계산합니다.
    size_t cursorPosition = 0;
    for (size_t i = 0; i < cursorRow; i++)
    {
        cursorPosition += linesOfEditControl[i].length();
    }
    cursorPosition += cursorCol;

    // 초성 입력
    if (chosungIndex.find(character) != chosungIndex.end() && (currentJungsung.empty() || (currentChosung.empty() && !currentJungsung.empty()) || !currentJongsung.empty()))
    {
        if (!currentChosung.empty() && currentJongsung.empty())
        {
            auto combinedJongIter = combinedJongsung.find({currentChosung, character});
            if (combinedJongIter != combinedJongsung.end())
            {
                std::string combinedJong = combinedJongIter->second;
                inputText = inputText.substr(0, cursorPosition - 3) + combinedJong + inputText.substr(cursorPosition);
                updateCursorPosition(0);
                currentJongsung2 = currentChosung;
                currentChosung.clear();
                currentJungsung.clear();
                currentJongsung = character;
                return;
            }
        }
        auto combinedJongIter = combinedJongsung.find({currentJongsung, character});
        if (combinedJongIter != combinedJongsung.end())
        {
            std::string combinedJong = combinedJongIter->second;
            char16_t combinedChar = composeHangul(chosungIndex[currentChosung], jungsungIndex[currentJungsung], jongsungIndex[combinedJong]);
            inputText = inputText.substr(0, cursorPosition - 3) + char16ToString(combinedChar) + inputText.substr(cursorPosition);
            updateCursorPosition(0);
            currentJongsung2 = currentJongsung;
            currentJongsung = character;
            return;
        }

        inputText.insert(cursorPosition, character);
        updateCursorPosition(3);
        currentChosung = character;
        currentJungsung.clear();
        currentJongsung.clear();
        return;
    }
    // 중성 입력
    else if (jungsungIndex.find(character) != jungsungIndex.end())
    {
        // 조합 가능한 중성인지 확인
        if (currentJongsung.empty())
        {
            auto combinedJungIter = combinedJungsung.find({currentJungsung, character});
            if (combinedJungIter != combinedJungsung.end())
            {
                std::string combinedJung = combinedJungIter->second;
                if (!currentChosung.empty())
                {
                    char16_t combinedChar = composeHangul(chosungIndex[currentChosung], jungsungIndex[combinedJung], 0);
                    inputText = inputText.substr(0, cursorPosition - 3) + char16ToString(combinedChar) + inputText.substr(cursorPosition);
                    updateCursorPosition(0);
                    currentJungsung = combinedJung;
                }
                else
                {
                    inputText = inputText.substr(0, cursorPosition - 3) + combinedJung + inputText.substr(cursorPosition);
                    updateCursorPosition(0);
                    currentJungsung = combinedJung;
                }
                return;
            }
        }
        if (!currentJongsung.empty()) // 종성이 있고, 종성에 해당하는 초성이 있는 경우
        {
            if (chosungIndex.find(currentJongsung) != chosungIndex.end())
            {
                char16_t combined;
                if (!currentChosung.empty())
                {
                    if (!currentJongsung2.empty())
                    {
                        combined = composeHangul(chosungIndex[currentChosung], jungsungIndex[currentJungsung], jongsungIndex[currentJongsung2]);
                        inputText = inputText.substr(0, cursorPosition - 3) + char16ToString(combined) + inputText.substr(cursorPosition);
                    }
                    else
                    {
                        combined = composeHangul(chosungIndex[currentChosung], jungsungIndex[currentJungsung], 0);
                        inputText = inputText.substr(0, cursorPosition - 3) + char16ToString(combined) + inputText.substr(cursorPosition);
                    }
                }
                else
                {
                    inputText = inputText.substr(0, cursorPosition - 3) + currentJongsung2 + inputText.substr(cursorPosition);
                    updateCursorPosition(0);
                }
                currentChosung = currentJongsung;
                currentJongsung.clear();
                combined = composeHangul(chosungIndex[currentChosung], jungsungIndex[character], 0);
                inputText.insert(cursorPosition, char16ToString(combined));
                updateCursorPosition(3);
                currentJungsung = character;
            }
        }
        else if (!currentChosung.empty() && currentJungsung.empty()) // 초성이 있고, 중성이 없는 경우
        {
            char16_t combined = composeHangul(chosungIndex[currentChosung], jungsungIndex[character], 0);
            inputText = inputText.substr(0, cursorPosition - 3) + char16ToString(combined) + inputText.substr(cursorPosition);
            updateCursorPosition(0);
            currentJungsung = character;
        }
        else // 초성이 없거나 초성, 중성이 있는 경우
        {
            inputText.insert(cursorPosition, character);
            updateCursorPosition(3);
            currentChosung.clear();
            currentJungsung = character;
        }
        return;
    }
    // 종성 입력
    else if (jongsungIndex.find(character) != jongsungIndex.end())
    {
        if (!currentChosung.empty() && !currentJungsung.empty()) // 초성, 중성 모두 있는 경우
        {
            if (!currentJongsung.empty())
            {
                auto combinedJongIter = combinedJongsung.find({currentJongsung, character});
                if (combinedJongIter != combinedJongsung.end())
                {
                    std::string combinedJong = combinedJongIter->second;
                    char16_t combinedChar = composeHangul(chosungIndex[currentChosung], jungsungIndex[currentJungsung], jongsungIndex[combinedJong]);
                    inputText = inputText.substr(0, cursorPosition - 3) + char16ToString(combinedChar) + inputText.substr(cursorPosition);
                    updateCursorPosition(0);
                    currentJongsung2 = currentJongsung;
                    currentJongsung = character;
                    return;
                }
            }
            char16_t combined = composeHangul(chosungIndex[currentChosung], jungsungIndex[currentJungsung], jongsungIndex[character]);
            inputText = inputText.substr(0, cursorPosition - 3) + char16ToString(combined) + inputText.substr(cursorPosition);
            updateCursorPosition(0);
            currentJongsung = character;
            currentJongsung2.clear();
        }
    }
    else if (character == "ㅃ" || character == "ㅉ" || character == "ㄸ")
    {
        inputText.insert(cursorPosition, character);
        updateCursorPosition(3);
        currentChosung = character;
        currentJungsung.clear();
        currentJongsung.clear();
    }
    else
    {
        // 한글 자모가 아닌 입력의 경우
        inputText.insert(cursorPosition, character);
        updateCursorPosition(character.length());
        currentChosung.clear();
        currentJungsung.clear();
        currentJongsung.clear();
    }
}
// 커서의 좌우 이동 함수
size_t moveCursorLeft(const string &line, size_t col)
{
    if (col == 0)
        return 0;

    size_t moveBack = 1;
    unsigned char byte = static_cast<unsigned char>(line[col - moveBack]);

    // UTF-8 문자의 시작 바이트가 아닌 경우 추가 이동
    while (moveBack < col && (byte & 0xC0) == 0x80)
    {
        moveBack++;
        byte = static_cast<unsigned char>(line[col - moveBack]);
    }
    col = col - moveBack;
    if (col == 0 && cursorRow > 0)
    {
        cursorRow--;
        col += linesOfEditControl[cursorRow].length();
    }

    return col;
}
size_t moveCursorRight(size_t col)
{
    const string &line = linesOfEditControl[cursorRow];

    if (col >= line.length())
    {
        // 현재 줄이 마지막 줄이 아니라면 다음 줄의 시작으로 이동
        if (cursorRow + 1 < linesOfEditControl.size())
        {
            cursorRow++;
            col = 0; // 다음 줄의 시작 위치
        }
        else
        {
            // 마지막 줄이면 현재 줄의 끝으로 이동
            return line.length();
        }
    }

    size_t moveAhead = 1;

    // ASCII 문자나 UTF-8 시작 바이트를 찾을 때까지 이동
    while (moveAhead + col < line.length() && (static_cast<unsigned char>(line[col + moveAhead]) & 0xC0) == 0x80)
    {
        moveAhead++;
    }

    return col + moveAhead;
}
// UTF-8 문자 경계를 확인하는 도우미 함수
size_t adjustForUTF8Boundary(const string &line, size_t col)
{
    size_t moveAhead = 0;
    while (moveAhead + col < line.length() && (static_cast<unsigned char>(line[col + moveAhead]) & 0xC0) == 0x80)
    {
        moveAhead++;
    }
    return col + moveAhead;
}
size_t selectionStartRow = 0;
size_t selectionStartCol = 0;
bool isSelecting = false;
bool isFocusedEditText = true;
bool isKoreanInput = true; // 한글 입력 모드 플래그
const float PI = 3.14159265359f;

void handleNumberAndSpecialKeys(int key, int mods)
{
    // 숫자 및 특수문자 처리
    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
    {
        char number = key - GLFW_KEY_0 + '0';
        if (mods & GLFW_MOD_SHIFT) // 특수기호로 변환
        {
            switch (number)
            {
            case '1':
                insertKey("!");
                break;
            case '2':
                insertKey("@");
                break;
            case '3':
                insertKey("#");
                break;
            case '4':
                insertKey("$");
                break;
            case '5':
                insertKey("%");
                break;
            case '6':
                insertKey("^");
                break;
            case '7':
                insertKey("&");
                break;
            case '8':
                insertKey("*");
                break;
            case '9':
                insertKey("(");
                break;
            case '0':
                insertKey(")");
                break;
            default:
                insertKey(std::string(1, number));
                break;
            }
        }
        else // 숫자로 출력
        {
            insertKey(std::string(1, number));
        }
    }
    // 기타 특수문자 처리
    else
    {
        std::string symbol;
        switch (key)
        {
        case GLFW_KEY_SPACE:
            symbol = " ";
            break;
        case GLFW_KEY_GRAVE_ACCENT:
            symbol = (mods & GLFW_MOD_SHIFT) ? "~" : "`";
            break;
        case GLFW_KEY_MINUS:
            symbol = (mods & GLFW_MOD_SHIFT) ? "_" : "-";
            break;
        case GLFW_KEY_EQUAL:
            symbol = (mods & GLFW_MOD_SHIFT) ? "+" : "=";
            break;
        case GLFW_KEY_LEFT_BRACKET:
            symbol = (mods & GLFW_MOD_SHIFT) ? "{" : "[";
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            symbol = (mods & GLFW_MOD_SHIFT) ? "}" : "]";
            break;
        case GLFW_KEY_BACKSLASH:
            symbol = (mods & GLFW_MOD_SHIFT) ? "|" : "\\";
            break;
        case GLFW_KEY_SEMICOLON:
            symbol = (mods & GLFW_MOD_SHIFT) ? ":" : ";";
            break;
        case GLFW_KEY_APOSTROPHE:
            symbol = (mods & GLFW_MOD_SHIFT) ? "\"" : "'";
            break;
        case GLFW_KEY_COMMA:
            symbol = (mods & GLFW_MOD_SHIFT) ? "<" : ",";
            break;
        case GLFW_KEY_PERIOD:
            symbol = (mods & GLFW_MOD_SHIFT) ? ">" : ".";
            break;
        case GLFW_KEY_SLASH:
            symbol = (mods & GLFW_MOD_SHIFT) ? "?" : "/";
            break;
        default:
            return; // 처리할 키가 아니면 함수 종료
        }
        insertKey(symbol);
    }
}
void handleArrowAndShiftKey(int mods)
{
    if (mods & GLFW_MOD_SHIFT)
    {
        if (isSelecting == false)
        {
            selectionStartRow = cursorRow;
            selectionStartCol = cursorCol;
            isSelecting = true;
        }
        else
        {
            return;
        }
    }
    else
    {
        isSelecting = false;
        currentChosung.clear();
        currentJungsung.clear();
        currentJongsung.clear();
    }
}
// 캐릭터 위치 변수 초기화
float characterPosX = 0.0f;
float characterPosZ = 5.0f;
const float moveSpeed = 0.05f; // 이동 속도
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // Shift 키의 상태를 확인
    bool shiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_BACKSPACE && cursorRow >= 0 && cursorCol > 0)
        {
            size_t cursorPosition = 0;
            for (int i = 0; i < cursorRow; i++)
            {
                cursorPosition += linesOfEditControl[i].length();
            }
            cursorPosition += cursorCol;

            if (isSelecting)
            {
                size_t startRowPosition = 0;
                for (int i = 0; i < selectionStartRow; i++)
                {
                    startRowPosition += linesOfEditControl[i].length();
                }
                size_t startSelectionPosition = startRowPosition + selectionStartCol;

                size_t endSelectionPosition = cursorPosition;

                if (startSelectionPosition > endSelectionPosition)
                {
                    std::swap(startSelectionPosition, endSelectionPosition);
                }

                inputText.erase(startSelectionPosition, endSelectionPosition - startSelectionPosition);
                linesOfEditControl = splitLines(inputText);
                cursorRow = min(cursorRow, selectionStartRow);
                cursorCol = min(cursorCol, selectionStartCol);

                // Selection is now cleared
                isSelecting = false;
            }
            else
            {
                // UTF-8 문자의 마지막 바이트에서 시작하여 문자의 시작을 찾음
                int eraseCount = 1;
                while (cursorPosition - eraseCount > 0 && (inputText[cursorPosition - eraseCount] & 0xC0) == 0x80)
                {
                    ++eraseCount;
                }

                // 지정된 위치의 문자를 삭제
                inputText.erase(cursorPosition - eraseCount, eraseCount);

                cursorCol -= eraseCount;
                // linesOfEditControl 및 커서 위치를 업데이트
                linesOfEditControl = splitLines(inputText);

                if (cursorCol == 0 && cursorRow > 0)
                {
                    cursorRow--;
                    cursorCol += linesOfEditControl[cursorRow].length();
                }
            }
        }
        else if (key == GLFW_KEY_ENTER)
        {
            // Enter 키가 눌렸을 때 실행할 코드
            if (shiftPressed)
            {
                insertKey("\n");
            }
            else
            {
                submit();
            }
            isSelecting = false;
            return;
        }
        else if (key == GLFW_KEY_TAB)
        {
            isFocusedEditText = !isFocusedEditText;
        }
        if (!shiftPressed && isSelecting) // 쉬프트 이외의 키가 눌렸을 때 isSelecting = false;
        {
            isSelecting = false;
        }
        if (key == GLFW_KEY_RIGHT_CONTROL) // 한글/영어 전환
        {
            isKoreanInput = !isKoreanInput;
            return;
        }
        if (isKoreanInput) // 한글 입력 모드
        {
            switch (key)
            {
            case GLFW_KEY_Q:
                if (mods & GLFW_MOD_SHIFT)
                {
                    insertKey("ㅃ");
                }
                else
                {
                    insertKey("ㅂ");
                }
                break;
            case GLFW_KEY_W:
                if (mods & GLFW_MOD_SHIFT)
                {
                    insertKey("ㅉ");
                }
                else
                {
                    insertKey("ㅈ");
                }
                break;
            case GLFW_KEY_E:
                if (mods & GLFW_MOD_SHIFT)
                {
                    insertKey("ㄸ");
                }
                else
                {
                    insertKey("ㄷ");
                }
                break;
            case GLFW_KEY_R:
                if (mods & GLFW_MOD_SHIFT)
                {
                    insertKey("ㄲ");
                }
                else
                {
                    insertKey("ㄱ");
                }
                break;
            case GLFW_KEY_T:
                if (mods & GLFW_MOD_SHIFT)
                {
                    insertKey("ㅆ");
                }
                else
                {
                    insertKey("ㅅ");
                }
                break;
            case GLFW_KEY_Y:
                insertKey("ㅛ");
                break;
            case GLFW_KEY_U:
                insertKey("ㅕ");
                break;
            case GLFW_KEY_I:
                insertKey("ㅑ");
                break;
            case GLFW_KEY_O:
                if (mods & GLFW_MOD_SHIFT)
                {
                    insertKey("ㅒ");
                }
                else
                {
                    insertKey("ㅐ");
                }
                break;
            case GLFW_KEY_P:
                if (mods & GLFW_MOD_SHIFT)
                {
                    insertKey("ㅖ");
                }
                else
                {
                    insertKey("ㅔ");
                }
                break;
            case GLFW_KEY_A:
                insertKey("ㅁ");
                break;
            case GLFW_KEY_S:
                insertKey("ㄴ");
                break;
            case GLFW_KEY_D:
                insertKey("ㅇ");
                break;
            case GLFW_KEY_F:
                insertKey("ㄹ");
                break;
            case GLFW_KEY_G:
                insertKey("ㅎ");
                break;
            case GLFW_KEY_H:
                insertKey("ㅗ");
                break;
            case GLFW_KEY_J:
                insertKey("ㅓ");
                break;
            case GLFW_KEY_K:
                insertKey("ㅏ");
                break;
            case GLFW_KEY_L:
                insertKey("ㅣ");
                break;
            case GLFW_KEY_Z:
                insertKey("ㅋ");
                break;
            case GLFW_KEY_X:
                insertKey("ㅌ");
                break;
            case GLFW_KEY_C:
                insertKey("ㅊ");
                break;
            case GLFW_KEY_V:
                insertKey("ㅍ");
                break;
            case GLFW_KEY_B:
                insertKey("ㅠ");
                break;
            case GLFW_KEY_N:
                insertKey("ㅜ");
                break;
            case GLFW_KEY_M:
                insertKey("ㅡ");
                break;
            }
        }
        else // 영어 입력 모드
        {
            if (key >= GLFW_KEY_A && key <= GLFW_KEY_Z)
            {
                char character = key - GLFW_KEY_A + 'A';
                if (mods & GLFW_MOD_SHIFT) // 대문자로 출력
                {
                    insertKey(std::string(1, character));
                }
                else // 소문자로 출력
                {
                    insertKey(std::string(1, static_cast<char>(tolower(character))));
                }
            }
        }
        if (isFocusedEditText)
        {
            // 숫자와 특수기호 처리
            handleNumberAndSpecialKeys(key, mods);
            // 방향키 처리
            if (inputText.length() > 0) // 입력된 텍스트가 있을 때만 방향키 처리
            {
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                {
                    if (cursorCol > 0)
                    {
                        handleArrowAndShiftKey(mods);
                        cursorCol = moveCursorLeft(linesOfEditControl[cursorRow], cursorCol);
                    }
                }
                else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                {
                    handleArrowAndShiftKey(mods);
                    cursorCol = moveCursorRight(cursorCol);
                }
                else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                {
                    if (cursorRow == 0)
                    {
                        cursorCol = 0;
                    }
                    else
                    {
                        cursorRow--;
                        cursorCol = min(cursorCol, linesOfEditControl[cursorRow].length());
                        cursorCol = adjustForUTF8Boundary(linesOfEditControl[cursorRow], cursorCol);
                    }
                    handleArrowAndShiftKey(mods);
                }
                else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                {
                    if (cursorRow == linesOfEditControl.size() - 1)
                    {
                        cursorCol = linesOfEditControl[cursorRow].length();
                    }
                    else
                    {
                        cursorRow++;
                        cursorCol = min(cursorCol, linesOfEditControl[cursorRow].length());
                        cursorCol = adjustForUTF8Boundary(linesOfEditControl[cursorRow], cursorCol);
                    }
                    handleArrowAndShiftKey(mods);
                }
            } else { // 입력된 텍스트가 없을 때는 채널 이동
                if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                {
                }
                else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                {
                }
                else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                {
                    ushort numch = numCh(cNode[1]);
                    if (numch > cCh[1])
                    {
                        cCh[1]++;
                    }
                    else
                    {
                        cCh[1] = 0;
                    }
                }
                else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                {
                    if (cCh[1] > 0)
                    {
                        cCh[1]--;
                    }
                    else
                    {
                        cCh[1] = numCh(cNode[1]);
                    }
                }
            }
        }
    }
}
// 라운드 처리된 사각형 그리기
void drawRoundedRectangle(float x, float y, float width, float height, float radius, int segments)
{
    glBegin(GL_LINE_LOOP);

    // Top-left corner
    for (int i = 3 * segments / 4; i <= segments; ++i)
    {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        glVertex2f(x + radius * (2 + sinf(angle)), y + height - radius * (2 - cosf(angle)));
    }

    // Top-right corner
    for (int i = 0; i <= segments / 4; ++i)
    {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        glVertex2f(x + width - radius * (2 - sinf(angle)), y + height - radius * (2 - cosf(angle)));
    }

    // Bottom-right corner
    for (int i = segments / 4; i <= segments / 2; ++i)
    {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        glVertex2f(x + width - radius * (2 - sinf(angle)), y + radius * (2 + cosf(angle)));
    }

    // Bottom-left corner
    for (int i = segments / 2; i <= 3 * segments / 4; ++i)
    {
        float angle = 2.0f * M_PI * float(i) / float(segments);
        glVertex2f(x + radius * (2 + sinf(angle)), y + radius * (2 + cosf(angle)));
    }

    glEnd();
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
// 셰이더 컴파일 함수
GLuint compileShader(const GLchar *shaderSource, GLenum shaderType)
{
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &shaderSource, nullptr);
    glCompileShader(shader);

    // 컴파일 에러 체크
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n"
                  << infoLog << std::endl;
    }

    return shader;
}
// 셰이더 프로그램 링크 함수
GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // 링크 에러 체크
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}
std::vector<float> generateCubeData(float sideLength, bool normalsOutward, float posX, float posY, float posZ)
{
    // 변의 길이에 따라 정점 좌표를 조정합니다.
    float halfSide = sideLength / 2.0f;

    // 법선이 바깥을 향하는지 여부에 따라 부호를 조정합니다.
    float normalDirection = normalsOutward ? 1.0f : -1.0f;

    // 정육면체의 정점, 법선 및 텍스처 좌표 데이터
    std::vector<float> cubeVertices = {
        // 위치                      // 법선                          // 텍스처 좌표
        // 앞면
        -halfSide + posX, -halfSide + posY, halfSide + posZ, 0.0f, 0.0f, normalDirection, 0.0f, 0.0f,
        halfSide + posX, -halfSide + posY, halfSide + posZ, 0.0f, 0.0f, normalDirection, 1.0f, 0.0f,
        halfSide + posX, halfSide + posY, halfSide + posZ, 0.0f, 0.0f, normalDirection, 1.0f, 1.0f,
        -halfSide + posX, halfSide + posY, halfSide + posZ, 0.0f, 0.0f, normalDirection, 0.0f, 1.0f,

        // 뒷면
        -halfSide + posX, -halfSide + posY, -halfSide + posZ, 0.0f, 0.0f, -normalDirection, 1.0f, 0.0f,
        halfSide + posX, -halfSide + posY, -halfSide + posZ, 0.0f, 0.0f, -normalDirection, 0.0f, 0.0f,
        halfSide + posX, halfSide + posY, -halfSide + posZ, 0.0f, 0.0f, -normalDirection, 0.0f, 1.0f,
        -halfSide + posX, halfSide + posY, -halfSide + posZ, 0.0f, 0.0f, -normalDirection, 1.0f, 1.0f,

        // 왼쪽 면
        -halfSide + posX, -halfSide + posY, -halfSide + posZ, -normalDirection, 0.0f, 0.0f, 0.0f, 0.0f,
        -halfSide + posX, -halfSide + posY, halfSide + posZ, -normalDirection, 0.0f, 0.0f, 1.0f, 0.0f,
        -halfSide + posX, halfSide + posY, halfSide + posZ, -normalDirection, 0.0f, 0.0f, 1.0f, 1.0f,
        -halfSide + posX, halfSide + posY, -halfSide + posZ, -normalDirection, 0.0f, 0.0f, 0.0f, 1.0f,

        // 오른쪽 면
        halfSide + posX, -halfSide + posY, -halfSide + posZ, normalDirection, 0.0f, 0.0f, 1.0f, 0.0f,
        halfSide + posX, -halfSide + posY, halfSide + posZ, normalDirection, 0.0f, 0.0f, 0.0f, 0.0f,
        halfSide + posX, halfSide + posY, halfSide + posZ, normalDirection, 0.0f, 0.0f, 0.0f, 1.0f,
        halfSide + posX, halfSide + posY, -halfSide + posZ, normalDirection, 0.0f, 0.0f, 1.0f, 1.0f,

        // 위쪽 면
        -halfSide + posX, halfSide + posY, -halfSide + posZ, 0.0f, normalDirection, 0.0f, 0.0f, 1.0f,
        -halfSide + posX, halfSide + posY, halfSide + posZ, 0.0f, normalDirection, 0.0f, 0.0f, 0.0f,
        halfSide + posX, halfSide + posY, halfSide + posZ, 0.0f, normalDirection, 0.0f, 1.0f, 0.0f,
        halfSide + posX, halfSide + posY, -halfSide + posZ, 0.0f, normalDirection, 0.0f, 1.0f, 1.0f,

        // 아래쪽 면
        -halfSide + posX, -halfSide + posY, -halfSide + posZ, 0.0f, -normalDirection, 0.0f, 1.0f, 1.0f,
        -halfSide + posX, -halfSide + posY, halfSide + posZ, 0.0f, -normalDirection, 0.0f, 0.0f, 1.0f,
        halfSide + posX, -halfSide + posY, halfSide + posZ, 0.0f, -normalDirection, 0.0f, 0.0f, 0.0f,
        halfSide + posX, -halfSide + posY, -halfSide + posZ, 0.0f, -normalDirection, 0.0f, 1.0f, 0.0f};

    return cubeVertices;
}
vector<unsigned int> cubeIndices = {
    // 앞면
    0, 1, 2,
    2, 3, 0,
    // 뒷면
    4, 5, 6,
    6, 7, 4,
    // 왼쪽 면
    8, 9, 10,
    10, 11, 8,
    // 오른쪽 면
    12, 13, 14,
    14, 15, 12,
    // 위쪽 면
    16, 17, 18,
    18, 19, 16,
    // 아래쪽 면
    20, 21, 22,
    22, 23, 20};
vector<float> cube2(float sideLength, float posX, float posY, float posZ)
{
    float halfSide = sideLength / 2.0f;

    // 위치, 법선, 텍스처 좌표를 포함하는 8개의 정점
    std::vector<float> cubeVertices = {
        // 위치                       // 법선                   // 텍스처 좌표
        // 정점 0: 왼쪽 아래 뒤
        -halfSide + posX, -halfSide + posY, -halfSide + posZ, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,
        // 정점 1: 오른쪽 아래 뒤
        halfSide + posX, -halfSide + posY, -halfSide + posZ, 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,
        // 정점 2: 오른쪽 위 뒤
        halfSide + posX, halfSide + posY, -halfSide + posZ, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
        // 정점 3: 왼쪽 위 뒤
        -halfSide + posX, halfSide + posY, -halfSide + posZ, -1.0f, 1.0f, -1.0f, 0.0f, 1.0f,
        // 정점 4: 왼쪽 아래 앞
        -halfSide + posX, -halfSide + posY, halfSide + posZ, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
        // 정점 5: 오른쪽 아래 앞
        halfSide + posX, -halfSide + posY, halfSide + posZ, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
        // 정점 6: 오른쪽 위 앞
        halfSide + posX, halfSide + posY, halfSide + posZ, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        // 정점 7: 왼쪽 위 앞
        -halfSide + posX, halfSide + posY, halfSide + posZ, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f};

    return cubeVertices;
}
std::vector<unsigned int> cube2Indices()
{
    std::vector<unsigned int> indices = {
        // 앞면
        0, 1, 2, 2, 3, 0,
        // 뒷면
        4, 5, 6, 6, 7, 4,
        // 왼쪽 면
        4, 7, 3, 3, 0, 4,
        // 오른쪽 면
        1, 5, 6, 6, 2, 1,
        // 위쪽 면
        3, 2, 6, 6, 7, 3,
        // 아래쪽 면
        4, 0, 1, 1, 5, 4};

    return indices;
}
// 구를 구성하는 정점들을 생성하는 함수
void createSphereVertices(std::vector<float> &vertices, unsigned int stacks, unsigned int slices, float radius, float posX, float posY, float posZ)
{
    for (unsigned int i = 0; i <= stacks; ++i)
    {
        // V 텍스처 좌표
        float V = (float)i / stacks;

        // 스택의 각도를 계산합니다. 여기서 pi는 원주율을 나타냅니다.
        float stackAngle = glm::pi<float>() / 2 - i * glm::pi<float>() / stacks;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (unsigned int j = 0; j <= slices; ++j)
        {
            // U 텍스처 좌표
            float U = (float)j / slices;

            // 슬라이스의 각도를 계산합니다.
            float sliceAngle = j * 2 * glm::pi<float>() / slices;
            float x = xy * cosf(sliceAngle);
            float y = xy * sinf(sliceAngle);

            // 정점을 추가합니다. (x, y, z)
            vertices.push_back(x + posX);
            vertices.push_back(y + posY);
            vertices.push_back(z + posZ);
            // 법선 벡터를 계산하고 추가합니다.
            glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
            // 텍스처 좌표를 추가합니다.
            vertices.push_back(U);
            vertices.push_back(V);
        }
    }
}
void createSphereIndices(std::vector<unsigned int> &indices, unsigned int stacks, unsigned int slices)
{
    for (unsigned int i = 0; i < stacks; ++i)
    {
        unsigned int k1 = i * (slices + 1);
        unsigned int k2 = k1 + slices + 1;

        for (unsigned int j = 0; j < slices; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stacks - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}
// 원기둥 정점 데이터를 생성하는 함수
void GenerateCylinder(float baseRadius, float topRadius, float height, int sectorCount, float xPos, float yPos, float zPos, std::vector<float> &vertices)
{
    float sectorStep = 2 * PI / sectorCount;
    float sectorAngle; // 각 부문의 각도

    // 중심 정점, 법선, 텍스처 좌표
    vertices.push_back(xPos);              // x
    vertices.push_back(yPos + height / 2); // y
    vertices.push_back(zPos);              // z
    vertices.push_back(0.0f); // nx
    vertices.push_back(1.0f); // ny
    vertices.push_back(0.0f); // nz
    vertices.push_back(0.5f); // s
    vertices.push_back(0.5f); // t

    // 원의 가장자리 정점 계산
    for (int i = 0; i < sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;

        float x = cos(sectorAngle);
        float z = sin(sectorAngle);

        // 가장자리 정점 추가
        vertices.push_back(xPos + x * topRadius); // x
        vertices.push_back(yPos + height / 2);    // y
        vertices.push_back(zPos + z * topRadius); // z

        // 가장자리 정점의 법선 (모두 동일)
        vertices.push_back(0.0f); // nx
        vertices.push_back(1.0f); // ny
        vertices.push_back(0.0f); // nz

        // 텍스처 좌표
        vertices.push_back((x + 1) * 0.5f); // s
        vertices.push_back((z + 1) * 0.5f); // t
    }
    // 중심 정점, 법선, 텍스처 좌표
    vertices.push_back(xPos);              // x
    vertices.push_back(yPos - height / 2); // y
    vertices.push_back(zPos);              // z
    vertices.push_back(0.0f);  // nx
    vertices.push_back(-1.0f); // ny
    vertices.push_back(0.0f);  // nz
    vertices.push_back(0.5f); // s
    vertices.push_back(0.5f); // t
    // 원의 가장자리 정점 계산
    for (int i = 0; i < sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;

        float x = cos(sectorAngle);
        float z = sin(sectorAngle);

        // 가장자리 정점 추가
        vertices.push_back(xPos + x * baseRadius); // x
        vertices.push_back(yPos - height / 2);     // y
        vertices.push_back(zPos + z * baseRadius); // z

        // 가장자리 정점의 법선 (모두 동일)
        vertices.push_back(0.0f);  // nx
        vertices.push_back(-1.0f); // ny
        vertices.push_back(0.0f);  // nz

        // 텍스처 좌표
        vertices.push_back((x + 1) * 0.5f); // s
        vertices.push_back((z + 1) * 0.5f); // t
    }
    // 옆면 정점 생성
    for (int i = 0; i < sectorCount; ++i)
    {
        sectorAngle = i * sectorStep;
        float x = cos(sectorAngle);
        float z = sin(sectorAngle);
        // 법선 벡터 계산
        // 측면 벡터의 x, z 성분은 동일하지만, y 성분은 기울기를 고려해야 함
        float slope = (baseRadius - topRadius) / height;          // 원기둥의 기울기
        float normalLength = sqrt(x * x + slope * slope + z * z); // 법선 벡터의 길이
        float nx = x / normalLength;                              // 법선 벡터의 x 성분
        float ny = slope / normalLength;                          // 법선 벡터의 y 성분 (기울기를 고려)
        float nz = z / normalLength;                              // 법선 벡터의 z 성분

        // 상단 원의 정점 추가 (원기둥의 위치를 고려하여 수정)
        vertices.push_back(xPos + x * topRadius); // x
        vertices.push_back(yPos + height / 2);    // y
        vertices.push_back(zPos + z * topRadius); // z

        // 상단 원의 정점 법선 추가 (y 방향)
        vertices.push_back(nx); // nx
        vertices.push_back(ny); // ny
        vertices.push_back(nz); // nz

        // 상단 원의 텍스처 좌표 추가
        vertices.push_back((x + 1) / 2); // s
        vertices.push_back((z + 1) / 2); // t

        // 하단 원의 정점 추가 (원기둥의 위치를 고려하여 수정)
        vertices.push_back(xPos + x * baseRadius); // x
        vertices.push_back(yPos - height / 2);     // y
        vertices.push_back(zPos + z * baseRadius); // z

        // 하단 원의 정점 법선 추가 (y 방향)
        vertices.push_back(nx); // nx
        vertices.push_back(ny); // ny
        vertices.push_back(nz); // nz

        // 하단 원의 텍스처 좌표 추가
        vertices.push_back((x + 1) / 2); // s
        vertices.push_back((z + 1) / 2); // t
    }
}
// 원기둥 인덱스를 생성하는 함수
void GenerateCylinderIndices(int sectorCount, std::vector<unsigned int> &indices)
{
    // 윗면 인덱스 생성
    for (int i = 1; i <= sectorCount; ++i)
    {
        indices.push_back(0);     // 원의 중심 정점의 인덱스
        indices.push_back(i);     // 현재 섹터의 첫 번째 정점
        indices.push_back(i + 1); // 현재 섹터의 다음 정점
    }
    // 마지막 삼각형을 닫기 위해
    indices.back() = 1; // 마지막 섹터의 다음 정점을 첫 번째 가장자리 정점으로 설정

    // 아랫면 인덱스 생성
    for (int i = sectorCount + 2; i <= 2 * sectorCount + 1; ++i)
    {
        indices.push_back(sectorCount + 1); // 원의 중심 정점의 인덱스
        indices.push_back(i);               // 현재 섹터의 첫 번째 정점
        indices.push_back(i + 1);           // 현재 섹터의 다음 정점
    }
    // 마지막 삼각형을 닫기 위해
    indices.back() = sectorCount + 2; // 마지막 섹터의 다음 정점을 첫 번째 가장자리 정점으로 설정

    // 옆면 인덱스 생성
    int k1, k2;
    for (int i = 0; i < sectorCount; ++i)
    {
        k1 = 2 * sectorCount + 2 + i * 2; // 상단 원의 현재 섹터의 첫 번째 정점 인덱스
        k2 = k1 + 1;                      // 하단 원의 현재 섹터의 첫 번째 정점 인덱스

        // 상단 원의 삼각형
        if (i < sectorCount - 1)
        {
            indices.push_back(k1);
            indices.push_back(k1 + 2);
            indices.push_back(k1 + 3);
            indices.push_back(k1);
            indices.push_back(k1 + 3);
            indices.push_back(k2);
        }
        else
        { // 마지막 삼각형에서는 마지막 정점과 첫 번째 정점을 연결
            indices.push_back(k1);
            indices.push_back(2 * sectorCount + 2);
            indices.push_back(2 * sectorCount + 3);
            indices.push_back(k1);
            indices.push_back(2 * sectorCount + 3);
            indices.push_back(k2);
        }
    }
}
const glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f); // x, y, z
glm::vec3 velocityVec = glm::vec3(0.0f, 0.0f, 0.0f);     // x, y, z
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};
class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Up;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
           float yaw = -90.0f, float pitch = 0.0f) : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
                                                     MovementSpeed(2.5f),
                                                     MouseSensitivity(0.1f),
                                                     Zoom(45.0f)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        updateCameraVectors();
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }
};
Camera camera(glm::vec3(0.0f, 3.0f, 3.0f));
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    static float lastX = 800 / 2.0f;
    static float lastY = 600 / 2.0f;
    static bool firstMouse = true;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y좌표는 아래에서 위로 갈수록 증가하기 때문에 반전시킴
    lastX = xpos;
    lastY = ypos;
    if (!isFocusedEditText)
    {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}
class Shader
{
public:
    // 생성자
    Shader(const GLchar *vertexPath, const GLchar *fragmentPath)
    {

        GLuint vertexShader = compileShader(vertexPath, GL_VERTEX_SHADER);
        GLuint fragmentShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
        ID = linkProgram(vertexShader, fragmentShader);
        // 1. 셰이더 코드를 파일에서 읽어옵니다.
        // 2. 셰이더를 컴파일합니다.
        // GLuint vertex, fragment;
        // int success;
        // char infoLog[512];
        // // 버텍스 셰이더
        // vertex = glCreateShader(GL_VERTEX_SHADER);
        // glShaderSource(vertex, 1, &vertexPath, NULL);
        // glCompileShader(vertex);
        // checkCompileErrors(vertex, "VERTEX");

        // // 프래그먼트 셰이더
        // fragment = glCreateShader(GL_FRAGMENT_SHADER);
        // glShaderSource(fragment, 1, &fragmentPath, NULL);
        // glCompileShader(fragment);
        // checkCompileErrors(fragment, "FRAGMENT");

        // // 셰이더 프로그램
        // ID = glCreateProgram();
        // glAttachShader(ID, vertex);
        // glAttachShader(ID, fragment);
        // glLinkProgram(ID);
        // checkCompileErrors(ID, "PROGRAM");

        // // 셰이더가 더 이상 필요하지 않으니 삭제합니다.
        // glDeleteShader(vertex);
        // glDeleteShader(fragment);
    }

    // 사용 메서드
    void use()
    {
        glUseProgram(ID);
    }

    // 유니폼 변수 설정 메서드
    void setMat4(const std::string &name, const glm::mat4 &mat)
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setvec3(const std::string &name, const glm::vec3 &vec3)
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), vec3.x, vec3.y, vec3.z);
    }
    GLuint program()
    {
        return ID;
    }

private:
    GLuint ID;

    // 셰이더 컴파일 오류를 확인하고 메시지를 출력합니다.
    void checkCompileErrors(GLuint shader, std::string type)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};
class SceneObject
{
public:
    GLuint VAO, VBO, EBO;
    glm::mat4 modelMatrix;

    // 생성자
    SceneObject(vector<float> vertices, vector<uint> indices)
    {
        // OpenGL 리소스를 설정하고 초기화합니다.
        // 예를 들어, VAO, VBO, EBO를 생성하고 바인딩하며 데이터를 업로드합니다.
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // 정점 속성 포인터 설정
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        // 법선 속성 설정
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0); // VAO 바인드 해제
    }

    // 소멸자
    ~SceneObject()
    {
        // OpenGL 리소스를 정리합니다.
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    glm::mat4 getModelMatrix() const
    {
        // 객체의 모델 행렬을 반환합니다. 여기서는 단순히 멤버 변수를 반환합니다.
        return modelMatrix;
    }

    void draw(Shader &shader, size_t indiceSize)
    {
        // 셰이더를 사용합니다.
        shader.use();
        // 유니폼 변수 설정
        shader.setMat4("model", modelMatrix);
        // VAO를 바인딩하고 렌더링합니다.
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indiceSize, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // 기타 필요한 메서드나 멤버 변수들이 여기에 추가될 수 있습니다.
};
GLuint createShader(const GLchar *vertex, const GLchar *fragment)
{
    GLuint vertexShader = compileShader(vertex, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragment, GL_FRAGMENT_SHADER);
    return linkProgram(vertexShader, fragmentShader);
}
void bind(unsigned int &VAO, std::vector<float> &vertices, std::vector<unsigned int> &indices)
{
    unsigned int VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 정점 위치 속성 포인터 설정
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // 법선 속성 설정
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 텍스처 좌표 속성 설정
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // VAO 바인드 해제
    // glDeleteBuffers(1, &VBO);
    // glDeleteBuffers(1, &EBO);
}
vector<uint> VAO;
std::vector<unsigned int> sphereIndices;
std::vector<unsigned int> cylinderIndices;
vector<uint> cube2Ind = cube2Indices();
void renderScene(uint &shader)
{
    //  구 그리기
    glBindVertexArray(VAO[0]);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // 바인드 해제

    glUniform3f(glGetUniformLocation(shader, "objectColor"), 0.5f, 0.5f, 0.31f);
    // 정육면체 그리기
    glBindVertexArray(VAO[1]);
    glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // 바인드 해제

    glUniform3f(glGetUniformLocation(shader, "objectColor"), 0.5f, 0.5f, 0.5f);
    glBindVertexArray(VAO[2]);
    glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // 바인드 해제

    glUniform3f(glGetUniformLocation(shader, "objectColor"), 0.6f, 0.4f, 0.3f);
    glBindVertexArray(VAO[3]);
    glDrawElements(GL_TRIANGLES, cylinderIndices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // 바인드 해제

    glUniform3f(glGetUniformLocation(shader, "objectColor"), 0.2f, 0.4f, 0.3f);
    glBindVertexArray(VAO[4]);
    glDrawElements(GL_TRIANGLES, cube2Ind.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // 바인드 해제
}
struct AABB
{
    glm::vec3 min; // 상자의 최소 좌표
    glm::vec3 max; // 상자의 최대 좌표
};

// AABB 충돌 감지 함수
bool CheckCollisionAABB(const AABB &a, const AABB &b)
{
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

// 캐릭터 이동 함수
void MoveCharacter(glm::vec3 &position, glm::vec3 &velocity, const AABB &characterBox, const std::vector<AABB> &worldBoxes)
{
    AABB newBox = characterBox; // 캐릭터의 새 위치에 대한 AABB
    newBox.min += velocity;
    newBox.max += velocity;

    for (const auto &worldBox : worldBoxes)
    {
        if (CheckCollisionAABB(newBox, worldBox))
        {
            // 충돌이 감지되면, 충돌 방향에 따라 속도를 조정
            if (velocity.x > 0 && newBox.max.x > worldBox.min.x)
            {
                velocity.x = 0;
            }
            else if (velocity.x < 0 && newBox.min.x < worldBox.max.x)
            {
                velocity.x = 0;
            }
            if (velocity.y > 0 && newBox.max.y > worldBox.min.y)
            {
                velocity.y = 0;
            }
            else if (velocity.y < 0 && newBox.min.y < worldBox.max.y)
            {
                velocity.y = 0;
            }
            if (velocity.z > 0 && newBox.max.z > worldBox.min.z)
            {
                velocity.z = 0;
            }
            else if (velocity.z < 0 && newBox.min.z < worldBox.max.z)
            {
                velocity.z = 0;
            }
            break; // 추가 충돌 처리를 방지하기 위해 반복 중단
        }
    }

    // 속도를 이용하여 캐릭터의 위치를 업데이트
    position += velocity;
}
// void LoadBitmapsFromTTF(const std::string &fontFilePath)
// {
//     FT_Library ft;
//     if (FT_Init_FreeType(&ft))
//     {
//         // FreeType 초기화 실패 처리
//         return;
//     }
//     FT_Face face;
//     if (FT_New_Face(ft, fontFilePath.c_str(), 0, &face))
//     {
//         // 폰트 로드 실패 처리
//         FT_Done_FreeType(ft);
//         return;
//     }
//     if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
//     {
//         // 인코딩 설정 오류 처리
//     }
//     FT_Set_Pixel_Sizes(face, 0, 12); // 폰트 크기 설정
//     int ii = 0;
//     vector<ushort> charCodeList;
//     for (int i = 0; i < 256; i++)
//     {
//         charCodeList.push_back(i);
//     }
//     for (int i = ttt; i < ttt + 256; i++)
//     {
//         // uint pCh = charTouint(CoRe[i] + 2); //Channel 0을 가리키는 포인터
//         uint startCoo = startCh(i, 0);
//         uint sizeCoo = charTouint(CoRe[i] + startCoo);
//         for (int j = 0; j < sizeCoo / 6; j++)
//         {
//             uint nextNode = charTouint(CoRe[i] + startCoo + 4 + j * 6);
//             uchar *temp = word(nextNode);
//             ushort temp2 = charToushort(temp + 4);
//             charCodeList.push_back(temp2);
//             delete[] temp;
//         }
//     }
//     // for (uint32_t charCode = 0x0000; charCode <= 0xFFFF; charCode++)
//     for (uint32_t charCode : charCodeList)
//     {
//         // ResetItalicTransform(face); // face에 기울기 없는 변환 적용
//         if (FT_Load_Char(face, charCode, FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL))
//         {
//             // 글리프 로드 실패 처리
//             continue;
//         }
//         ii++;
//         BitmapGlyphData data;
//         data.width = face->glyph->bitmap.width;
//         data.height = face->glyph->bitmap.rows;
//         data.advance = face->glyph->advance.x;
//         // 베어링 설정
//         data.bearing.x = face->glyph->bitmap_left;
//         data.bearing.y = face->glyph->bitmap_top;
//         data.bitmap = new unsigned char[data.width * data.height];
//         // 비트맵 데이터 복사 부분 수정
//         for (int y = 0; y < data.height; y++)
//         {
//             // pitch 값에 따라 올바른 행 시작 위치를 계산합니다.
//             int src_index = face->glyph->bitmap.pitch > 0 ? y * face->glyph->bitmap.pitch : (data.height - 1 - y) * -face->glyph->bitmap.pitch;
//             for (int x = 0; x < data.width; x++)
//             {
//                 data.bitmap[y * data.width + x] = face->glyph->bitmap.buffer[src_index + x];
//             }
//         }
//         // 맵에 저장
//         bitmapGlyphMap[charCode] = data;
//     }
//     FT_Done_Face(face);
//     FT_Done_FreeType(ft);
//     Log(L"글리프 개수 : " + intToWString(ii));
// }
// 2D 렌더링을 위한 투영 매트릭스 설정
void Setup2DProjection(int screenWidth, int screenHeight)
{
    glUseProgram(Shader_2d);
    // 직교 투영 매트릭스 생성
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(screenWidth), 0.0f, static_cast<float>(screenHeight));

    // 셰이더에 투영 매트릭스 설정
    glUniformMatrix4fv(glGetUniformLocation(Shader_2d, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUseProgram(0);
}
int main(int argc, char **argv)
{
    auto start = std::chrono::high_resolution_clock::now();
    // RAM에 Brain UpRoad
    ifstream in("/home/yonghwan/test/Save_and_BackUp/BackUp0/Brain3.bin", ios::binary);
    int ii = 0;
    uchar *size2 = new uchar[4];
    in.read(reinterpret_cast<char *>(size2), sizeof(uint));
    uint size3 = charTouint(size2);
    for (int i = 0; i < size3; i++)
    {
        uchar *size1 = new uchar[4];
        in.read(reinterpret_cast<char *>(size1), sizeof(uint));
        uint size = charTouint(size1);
        uchar *outer = new uchar[size + 4];
        outer[0] = size1[0];
        outer[1] = size1[1];
        outer[2] = size1[2];
        outer[3] = size1[3];
        delete[] size1;
        in.read(reinterpret_cast<char *>(&outer[4]), size);
        CoRe.push_back(outer);
    }
    in.close();

    string file_path2 = "/home/yonghwan/test/Save_and_BackUp/BackUp0/order2.bin";
    read_order(file_path2);
    cNode[1] = 0;
    cCh[1] = 1;
    thread t1(Network);
    GLFWwindow *window;

    /* GLFW 라이브러리 초기화 */
    if (!glfwInit())
        return -1;
    // GLFW 윈도우 설정
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    // glfwWindowHint(GLFW_DEPTH_BITS, 24); // 깊이 버퍼의 비트 수 설정
    /* 윈도우 생성 */
    window = glfwCreateWindow(1500, 1000, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* 컨텍스트를 현재 스레드에 바인딩 */
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // 화면 중앙에 커서 고정 및 커서 숨기기
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // GLEW 초기화
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW 초기화에 실패했습니다." << std::endl;
        return -1;
    }
    loadMapFromFile("/home/yonghwan/test/Save_and_BackUp/BackUp0/bitmapGlyphMap.bin", bitmapGlyphMap);


    for (const auto i : bitmapGlyphMap)
    {
        uchar *temp = uintToBytes(i.first);
        uint node = firstToken(temp, 2);
        delete[] temp;
        if (i.first > 256){
            uint iii = 0;
        }
        uchar temp2[5 + i.second.width * i.second.height];
        temp2[0] = i.second.width;
        temp2[1] = i.second.height;
        temp2[2] = i.second.advance >> 6;
        temp2[3] = i.second.bearing.x;
        temp2[4] = i.second.bearing.y;
        for (int ii = 0; ii < i.second.width * i.second.height; ii++)
        {
            temp2[5 + ii] = i.second.bitmap[ii];
        }
        
    }

    double lastTime = glfwGetTime();
    int numberOfFrames = 0;

    if (getMemoryUsage(vmSize, vmRSS))
    {
        std::cout << "VmSize: " << vmSize << " kB" << std::endl;
        std::cout << "VmRSS: " << vmRSS << " kB" << std::endl;
    }
    else
    {
        std::cerr << "Error reading memory usage." << std::endl;
    }

    // 뷰 포트 설정
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    uchar *sheetV = Sheet(37892); 
    uchar *sheetF = Sheet(37303);
    wstring now = charToWstring(sheetV);
    wstring now2 = charToWstring(sheetF);
    GLuint defaultShader = createShader(wstringToUtf8(now).c_str(), wstringToUtf8(now2).c_str());
    
    sheetV = Sheet(12946);
    sheetF = Sheet(36406);
    now = charToWstring(sheetV);
    now2 = charToWstring(sheetF);

    GLuint depthShader = createShader(wstringToUtf8(now).c_str(), wstringToUtf8(now2).c_str());
    sheetV = Sheet(39697);
    sheetF = Sheet(39040);
    now = charToWstring(sheetV);
    now2 = charToWstring(sheetF);
    delete[] sheetV;
    delete[] sheetF;

    Shader_2d = createShader(wstringToUtf8(now).c_str(), wstringToUtf8(now2).c_str());
    Setup2DProjection(width, height);

    std::vector<float> sphereVertices;
    createSphereVertices(sphereVertices, 20, 20, 1.0f, -2.0f, 1.0f, 2.0f);
    createSphereIndices(sphereIndices, 20, 20);
    vector<float> cylinderVertices;
    GenerateCylinder(1.0f, 0.5f, 1.0f, 5, 0.0f, 0.51f, 0.0f, cylinderVertices);
    GenerateCylinderIndices(5, cylinderIndices);

    // VAO설정
    unsigned int sphereVAO;
    bind(sphereVAO, sphereVertices, sphereIndices);
    VAO.push_back(sphereVAO);

    unsigned int cubeVAO;
    vector<float> vertices = generateCubeData(1.0f, true, 2.0f, 1.0f, 0.0f);
    bind(cubeVAO, vertices, cubeIndices);
    VAO.push_back(cubeVAO);

    unsigned int floorVAO;
    vertices = generateCubeData(20.0f, false, 0.0f, 10.0f, 0.0f);
    bind(floorVAO, vertices, cubeIndices);
    VAO.push_back(floorVAO);
    uint cylinderVAO;
    bind(cylinderVAO, cylinderVertices, cylinderIndices);
    VAO.push_back(cylinderVAO);
    uint cube2VAO;
    vertices = cube2(1.0f, 4.0f, 0.5f, 3.0f);
    bind(cube2VAO, vertices, cube2Ind);
    VAO.push_back(cube2VAO);

    // 광원 위치 초기화 (예: 천장의 중앙)
    glm::vec3 lightPos(0.0f, 9.0f, 0.0f);

    float deltaTime, lastFrame;
    // 깊이 맵 FBO 생성
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

    // 깊이 텍스처 생성
    unsigned int depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {0.0, 0.0, 0.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // FBO에 깊이 텍스처 첨부
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    const float jumpSpeed = 5.0f; // 점프 속도

    bool isGrounded = false; // 캐릭터가 땅에 있는지의 상태
    auto end = std::chrono::high_resolution_clock::now();
    // 경과 시간 계산 (밀리초 단위)
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    // 경과 시간 출력 (밀리초 단위)
    Log(L"로딩 시간: " + intToWString(duration.count()) + L"ms");
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix;
    glm::mat4 model = glm::mat4(1.0f);
    float near_plane = 0.1f, far_plane = 15.0f;
    // 광원의 투영 행렬을 생성합니다 (여기에서는 직교 투영을 사용합니다)
    lightProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, near_plane, far_plane);

    // 뷰와 투영 행렬을 결합하여 광원 공간 행렬을 얻습니다
    lightSpaceMatrix = lightProjection * lightView;

    /* 루프 조건 설정 */
    while (!glfwWindowShouldClose(window))
    {
        glEnable(GL_DEPTH_TEST);

        // 시간 계산
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // 중력 적용
        if (!isGrounded)
        { // 캐릭터가 공중에 있을 때만 중력 적용
            velocityVec += gravity * deltaTime;
        }
        // 입력 처리
        if (!isFocusedEditText)
        {
            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                camera.ProcessKeyboard(FORWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
                camera.ProcessKeyboard(BACKWARD, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                camera.ProcessKeyboard(LEFT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                camera.ProcessKeyboard(RIGHT, deltaTime);
            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded)
            {
                velocityVec.y = jumpSpeed; // 수직 속도에 점프 속도 추가
                isGrounded = false;        // 공중에 떠있음을 표시
            }
        }
        camera.Position += velocityVec * deltaTime;
        // 땅에 닿았는지 검사
        if (camera.Position.y < 1.0f)
        {
            camera.Position.y = 1.0f; // 캐릭터를 땅 위에 위치시키기
            velocityVec.y = 0.0f;     // y축 속도를 0으로 설정하여 더 이상 떨어지지 않게 함
            isGrounded = true;        // 캐릭터가 땅에 있음을 표시
        }
        // 깊이 맵을 렌더링하는 셰이더를 활성화합니다
        glClear(GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glUseProgram(depthShader);
        // 1. 광원의 관점에서 깊이 맵 렌더링
        // 광원의 위치와 타겟 설정 (방향성 광원의 경우 타겟은 보통 장면의 중심)
        glm::vec3 lightTarget = glm::vec3{0.0f, 0.0f, 0.0f}; // 광원이 바라보는 타겟 위치
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);          // 광원의 "위" 방향을 정의합니다
        // 광원의 관점에서 뷰 행렬을 생성합니다
        lightView = glm::lookAt(lightPos, lightTarget, up);
        // 셰이더에 유니폼 변수 설정
        glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniformMatrix4fv(glGetUniformLocation(depthShader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        //  구 그리기
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // 바인드 해제
        // 정육면체 그리기
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // 바인드 해제
        glBindVertexArray(VAO[2]);
        glDrawElements(GL_TRIANGLES, cubeIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0); // 바인드 해제//
        // FBO에서 기본 프레임버퍼로 다시 바인딩합니다
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // // 렌더링 명령
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 셰이더 사용
        glUseProgram(defaultShader);
        glViewport(0, 0, width, height);

        // 광원 위치 및 색상 설정
        glUniform3f(glGetUniformLocation(defaultShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
        glUniform3f(glGetUniformLocation(defaultShader, "lightColor"), 1.0f, 1.0f, 1.0f); // 백색 광원
        glUniform3f(glGetUniformLocation(defaultShader, "viewPos"), camera.Position.x, camera.Position.y, camera.Position.z);

        glUniformMatrix4fv(glGetUniformLocation(defaultShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        //  뷰 행렬 설정
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(defaultShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(defaultShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

        // 프로젝션 행렬 설정
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)width / (float)height, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(defaultShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3f(glGetUniformLocation(defaultShader, "objectColor"), 1.0f, 0.5f, 0.31f);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        // 셰이더에 그림자 맵 유니폼 변수를 전달
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUniform1i(glGetUniformLocation(defaultShader, "shadowMap"), 0); // GL_TEXTURE0에 해당하는 유니폼 위치에 0을 전달

        renderScene(defaultShader);
        // 2D 렌더링을 위해 깊이 테스트를 비활성화합니다.
        glDisable(GL_DEPTH_TEST);
        display(cNode[1], cCh[1]);
        getMemoryUsage(vmSize, vmRSS);
        info();
        Log(L"");

        // // 2D 렌더링
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, width, 0, height, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        double cursorTime = glfwGetTime();                           // 현재 시간 가져오기 (예: GLFW 사용)
        bool showCursor = static_cast<int>(cursorTime * 2) % 2 == 0; // 0.5초마다 깜빡이게 변경

        const float lineHeight = 15; // 줄 간격을 15 픽셀로 설정
        int lsize = linesOfEditControl.size();
        float boxX = 195;                          // 텍스트 상자 시작 위치 X
        float boxY = 10;                           // 텍스트 상자 시작 위치 Y
        float boxWidth = 410;                      // 텍스트 박스 너비
        float boxHeight = lsize * lineHeight + 20; // 텍스트 박스 높이
        float borderRadius = 5;                    // 라운드 처리된 테두리 반지름
        int segments = 12;                         // 원호의 세그먼트 수

        // 라운드 처리된 사각형 그리기
        glColor3f(0.9f, 0.9f, 0.9f); // 검정색 설정
        drawRoundedRectangle(boxX, boxY, boxWidth, boxHeight, borderRadius, segments);

        for (int i = lsize - 1; i >= 0; --i)
        {
            if (isSelecting && (i >= min(selectionStartRow, cursorRow) && i <= max(selectionStartRow, cursorRow)))
            {
                glColor3f(0.7f, 0.9f, 1.0f); // 연한 파란색 설정
                size_t lineStart = 0;
                size_t lineEnd = linesOfEditControl[i].length();

                if (i == selectionStartRow)
                {
                    if (i == cursorRow)
                    {
                        lineStart = min(selectionStartCol, cursorCol);
                        lineEnd = max(selectionStartCol, cursorCol);
                    }
                    else if (i < cursorRow)
                    {
                        lineStart = selectionStartCol;
                    }
                    else
                    {
                        lineEnd = selectionStartCol;
                    }
                }
                else if (i == cursorRow)
                {
                    if (i < selectionStartRow)
                    {
                        lineStart = cursorCol;
                    }
                    else
                    {
                        lineEnd = cursorCol;
                    }
                }

                // float highlightX = 200 + font.Advance(linesOfEditControl[i].substr(0, lineStart).c_str());
                float highlightX = 200 + bitmapGlyphMap[linesOfEditControl[i][lineStart]].advance;
                // float highlightWidth = font.Advance(linesOfEditControl[i].substr(lineStart, lineEnd - lineStart).c_str());
                float highlightWidth = bitmapGlyphMap[linesOfEditControl[i][lineStart]].advance;

                float highlightHeight = lineHeight;
                float highlightY = 20 + (lsize - 1 - i) * lineHeight;

                glBegin(GL_QUADS);
                glVertex2f(highlightX, highlightY);
                glVertex2f(highlightX + highlightWidth, highlightY);
                glVertex2f(highlightX + highlightWidth, highlightY + highlightHeight);
                glVertex2f(highlightX, highlightY + highlightHeight);
                glEnd();
            }
            RenderText(linesOfEditControl[i], 200.0f, 22.0f + (lsize - 1 - i) * lineHeight);
        }
        // 커서 렌더링
        if (showCursor && lsize > 0)
        {
            string textBeforeCursor = linesOfEditControl[cursorRow].substr(0, cursorCol);
            // float cursorX = 200 + font.Advance(textBeforeCursor.c_str());
            // float cursorX = 200 + bitmapGlyphMap[textBeforeCursor[textBeforeCursor.length() - 1]].advance;
            float cursorX = 200 + stringAdvance(textBeforeCursor);
            float cursorY = 20 + (linesOfEditControl.size() - 1 - cursorRow) * lineHeight;
            glColor3f(0.0f, 0.0f, 0.0f);
            glBegin(GL_LINES);
            glVertex2f(cursorX, cursorY);
            glVertex2f(cursorX, cursorY + 12);
            glEnd();
        }
        // FPS 계산
        double currentTime = glfwGetTime();
        numberOfFrames++;
        if (currentTime - lastTime >= 1.0)
        {
            fps = numberOfFrames;
            numberOfFrames = 0; // 프레임 수 재설정
            lastTime += 1.0;    // 마지막 시간 업데이트
        }
        /* 스왑 버퍼 */
        glfwSwapBuffers(window);
        /* 이벤트 폴링 */
        glfwPollEvents();
    }
    // 자원 해제
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteProgram(defaultShader);
    glDeleteProgram(depthShader);
    glDeleteProgram(Shader_2d);
    glDeleteTextures(1, &depthMap);
    glDeleteFramebuffers(1, &depthMapFBO);
    glDeleteVertexArrays(1, &sphereVAO);

    // glDeleteBuffers(1, &cubeVBO);
    // glDeleteProgram(shaderProgram);
    //~defaultShader();

    glfwDestroyWindow(window);
    glfwTerminate();
    t1.join();
    return 0;
}