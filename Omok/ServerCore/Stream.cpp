#include "pch.h"
#include "Stream.h"

/*-------------------------
	OutputMemoryStream
---------------------------*/
void OutputMemoryStream::ReallocBuffer(uint32_t inNewLength)
{
	_buffer = static_cast<BYTE*>(::realloc(_buffer, inNewLength));
	ASSERT_CRASH(_buffer != nullptr);
	_capacity = inNewLength;
}

void OutputMemoryStream::Write(const void* inData, size_t inByteCount)
{
	uint32_t resultHead = _head + static_cast<uint32_t>(inByteCount);
	if (resultHead > _capacity) {
		ReallocBuffer(max(_capacity * 2, resultHead));
	}
	::memcpy(_buffer + _head, inData, inByteCount);
	_head = resultHead;
}

/*-------------------------
	InputMemoryStream
---------------------------*/
void InputMemoryStream::Read(void* outData, uint32_t inByteCount)
{
	uint32_t resultHead = _head + inByteCount;
	if (resultHead > _capacity) {
		//error
	}
	::memcpy(outData, _buffer + _head, inByteCount);
	_head = resultHead;
}


/*---------------------------
		PacketFunc
-----------------------------*/

void OutputMemoryStream::WritePacketHeader(PacketHeader header)
{
	this->Write(header.totalSize);
	this->Write(header.packetId);
}

void OutputMemoryStream::WriteRoomEnterRes(PacketHeader header, uint32 errCode, BYTE* user1, BYTE* user2, BYTE* roomName)
{
	this->WritePacketHeader(header);
	this->Write(errCode);
	this->Write(user1, MAX_USER_NAME_SIZE);
	this->Write(user2, MAX_USER_NAME_SIZE);
	this->Write(roomName, MAX_USER_NAME_SIZE);
}