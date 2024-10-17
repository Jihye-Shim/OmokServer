#pragma once

/*-------------------------
	OutputMemoryStream
---------------------------*/
class OutputMemoryStream
{
public:
	OutputMemoryStream() : _buffer(nullptr), _head(0), _capacity(0) {
		ReallocBuffer(32);
	}
	~OutputMemoryStream() { free(_buffer); }

	BYTE* GetBufferPtr() { return _buffer; }
	int32 GetLength() const { return _head; }

	void Write(const void* inData, size_t inByteCount);
	void Write(uint32_t inData) { Write(&inData, sizeof(inData)); }
	void Write(int32_t inData) { Write(&inData, sizeof(inData)); }
	void Write(uint16_t inData) { Write(&inData, sizeof(inData)); }
	
	template<typename T>
	void Write(vector<T>& inVector, size_t len) { // container vector Á÷·ÄÈ­
		size_t elementCount = inVector.size();
		Write(elementCount);
		for (T& element : inVector)
			Write(element, len);
	}

	template<typename T> 
	void Write(const T& inData) {
		ASSERT_CRASH(::is_arithmetic<T>::value || ::is_enum<T>::value);
		Write(&inData, sizeof(inData));
	}

private:
	void ReallocBuffer(uint32_t inNewLength);

	BYTE* _buffer;
	uint32_t _head;
	uint32_t _capacity;
};

/*-------------------------
	InputMemoryStream
---------------------------*/
class InputMemoryStream
{
public:
	InputMemoryStream(BYTE* inBuffer, uint32_t inByteCount) : _buffer(inBuffer), _capacity(inByteCount), _head(0)
	{}
	~InputMemoryStream() { }

	uint32_t GetRemainingDataSize() const { return _capacity - _head; }

	void Read(void* outData, uint32_t inByteCount);
	void Read(uint32_t& outData) { Read(&outData, sizeof(outData)); }
	void Read(int32_t& outData) { Read(&outData, sizeof(outData)); }
	
	void Read(uint16_t& outData) { Read(&outData, sizeof(outData)); }

private:
	BYTE* _buffer;
	uint32_t _head;
	uint32_t _capacity;
};