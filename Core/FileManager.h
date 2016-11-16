#pragma once
#include <vector>
#include "../Util/FileClasses.h"
#include "SymbolData.h"

enum class Endianness { Big, Little };

class AssemblerFile
{
public:
	virtual ~AssemblerFile() { };
	
	virtual bool open(bool onlyCheck) = 0;
	virtual void close() = 0;
	virtual bool isOpen() = 0;
	virtual bool write(void* data, size_t length) = 0;
	virtual u64 getVirtualAddress() = 0;
	virtual u64 getPhysicalAddress() = 0;
	virtual size_t getHeaderSize() = 0;
	virtual bool seekVirtual(u64 virtualAddress) = 0;
	virtual bool seekPhysical(u64 physicalAddress) = 0;
	virtual bool getModuleInfo(SymDataModuleInfo& info) { return false; };
	virtual bool hasFixedVirtualAddress() { return false; };
	virtual void beginSymData() { };
	virtual void endSymData() { };
	virtual const std::wstring& getFileName() = 0;
};

class GenericAssemblerFile: public AssemblerFile
{
public:
	GenericAssemblerFile(const std::wstring& fileName, u32 headerSize, bool overwrite);
	GenericAssemblerFile(const std::wstring& fileName, const std::wstring& originalFileName, u32 headerSize);

	virtual bool open(bool onlyCheck);
	virtual void close() { if (handle.isOpen()) handle.close(); };
	virtual bool isOpen() { return handle.isOpen(); };
	virtual bool write(void* data, size_t length);
	virtual u64 getVirtualAddress() { return virtualAddress; };
	virtual u64 getPhysicalAddress() { return virtualAddress-headerSize; };
	virtual size_t getHeaderSize() { return headerSize; };
	virtual bool seekVirtual(u64 virtualAddress);
	virtual bool seekPhysical(u64 physicalAddress);
	virtual bool hasFixedVirtualAddress() { return true; };

	virtual const std::wstring& getFileName() { return fileName; };
	const std::wstring& getOriginalFileName() { return originalName; };
	size_t getOriginalHeaderSize() { return originalHeaderSize; };
	void setHeaderSize(size_t size) { headerSize = size; };
private:
	enum Mode { Open, Create, Copy };

	Mode mode;
	size_t originalHeaderSize;
	size_t headerSize;
	u64 virtualAddress;
	BinaryFile handle;
	std::wstring fileName;
	std::wstring originalName;
};


class FileManager
{
public:
	FileManager();
	~FileManager();
	void reset();
	bool openFile(AssemblerFile* file, bool onlyCheck);
	void addFile(AssemblerFile* file);
	bool hasOpenFile() { return activeFile != NULL; };
	void closeFile();
	bool write(void* data, size_t length);
	bool writeU8(u8 data);
	bool writeU16(u16 data);
	bool writeU32(u32 data);
	bool writeU64(u64 data);
	u64 getVirtualAddress();
	u64 getPhysicalAddress();
	size_t getHeaderSize();
	bool seekVirtual(u64 virtualAddress);
	bool seekPhysical(u64 physicalAddress);
	bool advanceMemory(size_t bytes);
	AssemblerFile* getOpenFile() { return activeFile; };
	void setEndianness(Endianness endianness) { this->endianness = endianness; };
	Endianness getEndianness() { return endianness; }
private:
	bool checkActiveFile();
	std::vector<AssemblerFile*> files;
	AssemblerFile* activeFile;
	Endianness endianness;
	Endianness ownEndianness;
};
