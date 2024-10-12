#include "pch.h"
#include "GameSession.h"

void GameSession::OnConnected(){
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
	cout << "Connect!" << endl;
}

void GameSession::OnSend(int32 numOfBytes){
	cout << "Send Len: " << numOfBytes << endl;
}

void GameSession::OnRecv(int32 numOfBytes) {
//	cout << "Recv" << endl;
//	GSessionManager.Broadcast(_recvBuffer);
	InputMemoryStream inStream(_recvBuffer, numOfBytes);
	PacketHeader inHeader;
	inStream.Read(inHeader.totalSize);
	if (inHeader.totalSize > numOfBytes) {
		//패킷 다 안 왔을 때 처리(recvBuffer 업데이트 필요)
	}
	
	inStream.Read(inHeader.packetId);
	OutputMemoryStreamRef outStream = make_shared<OutputMemoryStream>();
	OutputMemoryStreamRef temp = make_shared<OutputMemoryStream>();
	PacketId id = static_cast<PacketId>(inHeader.packetId);
	if(id == PacketId::Login_Req){
		PacketHeader header = MakeHeader<PacketLogInRes>(PacketId::Login_Res);
		PacketLogInReq pkt;
		inStream.Read(pkt.userName, MAX_USER_NAME_SIZE);
		WCHAR* wName = utf8ToWChar(pkt.userName);
		wcout << L"이름: " << wName << endl;

		outStream->Write(header.totalSize);
		cout << "LoginRes size: " << header.totalSize << endl;
		outStream->Write(header.packetId);
		outStream->Write((uint32)0);
		Send(outStream);

		header = MakeHeader<PacketLobbyEnterBroad>(PacketId::Lobby_Enter_Broad);
		temp->Write(header.totalSize);
		cout << "LobbyEnter size: " << header.totalSize << endl;
		temp->Write(header.packetId);
		temp->Write(pkt.userName, MAX_USER_NAME_SIZE);
		GSessionManager.Broadcast(temp);
	}
	else if (id == PacketId::Lobby_Chat_Req) {
		PacketHeader header = MakeHeader<PacketLobbyChatRes>(PacketId::Lobby_Chat_Res);
		PacketLobbyChatReq pkt;
		inStream.Read(pkt.userName, MAX_USER_NAME_SIZE);
		inStream.Read(pkt.msg, MAX_CHAT_SIZE);
		WCHAR* wName = utf8ToWChar(pkt.userName);
		WCHAR* wMsg = utf8ToWChar(pkt.msg);
		wcout << L"접속자 이름 : " << wName << L" msg: " << wMsg << endl;

		outStream->Write(header.totalSize);
		cout << "LobbyChatRes size: " << header.totalSize << endl;
		outStream->Write(header.packetId);
		outStream->Write((uint32)0);
		Send(outStream);

		header = MakeHeader<PacketLobbyChatBroad>(PacketId::Lobby_Chat_Broad);
		temp->Write(header.totalSize);
		cout << "Lobby Chat Broad size: " << header.totalSize << endl;
		temp->Write(header.packetId);
		temp->Write(pkt.userName, MAX_USER_NAME_SIZE);
		temp->Write(pkt.msg, MAX_CHAT_SIZE);
		GSessionManager.Broadcast(temp);
	}
}

void GameSession::OnDisconnected() {
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

WCHAR* GameSession::utf8ToWChar(BYTE* utf8)
{
	string utf8ToString(reinterpret_cast<char*>(utf8), strlen((char*)utf8));
	int sLen = MultiByteToWideChar(CP_UTF8, 0, utf8ToString.c_str(), utf8ToString.length(), NULL, 0);
	wchar_t* wideString = new wchar_t[sLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, utf8ToString.c_str(), utf8ToString.length(), OUT wideString, sLen);
	wideString[sLen] = '\0';
	return wideString;
}

