#include "pch.h"
#include "GameSession.h"
#include "Room.h"
#include "RoomManager.h"

void GameSession::OnConnected(){
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
	//cout << "Connect " << GetAddress().GetAdddr() << endl;
	cout << "Connect new user" << endl;
}

void GameSession::OnSend(int32 numOfBytes){
//	cout << "Send Len: " << numOfBytes << endl;
}

void GameSession::OnRecv(int32 numOfBytes) {
//	cout << "Recv" << endl;
	InputMemoryStreamRef inStream = make_shared<InputMemoryStream>(_recvBuffer, numOfBytes);
	PacketHeader inHeader;
	inStream->Read(inHeader.totalSize);
	if (inHeader.totalSize > numOfBytes) {
	}
	inStream->Read(inHeader.packetId);
	PacketId id = static_cast<PacketId>(inHeader.packetId);
	ProcessPacket(inStream, id);
}

void GameSession::OnDisconnected() {
	// 나머지 session에 접속 종료된 유저 통보
	PacketHeader header = MakeHeader<PacketLobbyLeaveBroad>(PacketId::Lobby_Leave_Broad);
	PacketLobbyLeaveBroad pkt;
	OutputMemoryStreamRef outStream = make_shared<OutputMemoryStream>();
	outStream->WritePacketHeader(header);
	outStream->Write(_name, MAX_USER_NAME_SIZE);
	GSessionManager.Broadcast(outStream);

	if (_room != nullptr) {
		_room->_session1 = nullptr;
		_room->_session2 = nullptr;
		GRoomManager.Remove(_room);
		_room = nullptr;
	}

	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this())); // 삭제
}

WCHAR* GameSession::Utf8ToWChar(BYTE* utf8)
{
	string utf8ToString(reinterpret_cast<char*>(utf8), strlen((char*)utf8));
	int32 sLen = MultiByteToWideChar(CP_UTF8, 0, utf8ToString.c_str(), utf8ToString.length(), NULL, 0);
	WCHAR* wideString = new WCHAR[sLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, utf8ToString.c_str(), utf8ToString.length(), OUT wideString, sLen);
	wideString[sLen] = '\0';
	return wideString;
}

void GameSession::ProcessPacket(InputMemoryStreamRef inStream, PacketId id)
{
	OutputMemoryStreamRef outStream = make_shared<OutputMemoryStream>();
	OutputMemoryStreamRef temp = make_shared<OutputMemoryStream>();
	if (id == PacketId::Login_Req) {
		PacketLogInReq pkt;
		inStream->Read(pkt.userName, MAX_USER_NAME_SIZE);
		WCHAR* wName = Utf8ToWChar(pkt.userName);
		wcout << L"Enter User Name: " << wName << endl;
		uint32 errCode;
		errCode = 0;
		::memcpy(_name, pkt.userName, MAX_USER_NAME_SIZE);

		PacketLogInRes resPkt;
		for (auto session : GSessionManager._sessions) {
			resPkt.userList.push_back(session->_name);
		}
		int cnt = 0;
		for (auto room : GRoomManager._rooms) {
			if (room->_session2 == nullptr) {
				resPkt.roomList.push_back(room->_name);
				cnt++;
			}
		}
		uint16 size = sizeof(PacketHeader) + sizeof(PacketBase) + sizeof(size_t) + MAX_USER_NAME_SIZE * resPkt.userList.size() + sizeof(size_t) + MAX_USER_NAME_SIZE * cnt;
		PacketHeader header = MakeHeader<PacketLogInRes>(PacketId::Login_Res);
		header.totalSize = size;

		outStream->WritePacketHeader(header);
//		cout << "LoginRes size: " << header.totalSize << endl;
		outStream->Write(errCode);
		outStream->Write(resPkt.userList, MAX_USER_NAME_SIZE);
		outStream->Write(resPkt.roomList, MAX_USER_NAME_SIZE);
		Send(outStream);

		header = MakeHeader<PacketLobbyEnterBroad>(PacketId::Lobby_Enter_Broad);
		temp->WritePacketHeader(header);
//		cout << "LobbyEnter size: " << header.totalSize << endl;
		temp->Write(pkt.userName, MAX_USER_NAME_SIZE);
		GSessionManager.Broadcast(temp);
	}
	else if (id == PacketId::Lobby_Chat_Req) {
		PacketHeader header = MakeHeader<PacketLobbyChatRes>(PacketId::Lobby_Chat_Res);
		PacketLobbyChatReq pkt;
		inStream->Read(pkt.userName, MAX_USER_NAME_SIZE);
		inStream->Read(pkt.msg, MAX_CHAT_SIZE);
		WCHAR* wName = Utf8ToWChar(pkt.userName);
		WCHAR* wMsg = Utf8ToWChar(pkt.msg);
		wcout << L"User: " << wName << L" Msg: " << wMsg << endl;

		outStream->WritePacketHeader(header);
//		cout << "LobbyChatRes size: " << header.totalSize << endl;
		outStream->Write((uint32)0);
		Send(outStream);

		header = MakeHeader<PacketLobbyChatBroad>(PacketId::Lobby_Chat_Broad);
		temp->WritePacketHeader(header);
//		cout << "Lobby Chat Broad size: " << header.totalSize << endl;
		temp->Write(pkt.userName, MAX_USER_NAME_SIZE);
		temp->Write(pkt.msg, MAX_CHAT_SIZE);
		GSessionManager.Broadcast(temp);
	}
	else if (id == PacketId::Room_Enter_Req) {
		PacketHeader header = MakeHeader<PacketLobbyChatRes>(PacketId::Room_Enter_Req);
		PacketRoomEnterReq pkt;
		inStream->Read(pkt.userName, MAX_USER_NAME_SIZE);
		inStream->Read(pkt.roomName, MAX_USER_NAME_SIZE);
		bool isExist = false;
		for (auto room : GRoomManager._rooms) {
			if (::memcmp(room->_name, pkt.roomName, sizeof(room->_name)) == 0) {
				isExist = true;
				room->_session2 = shared_from_this();
				this->_room = room;
				WCHAR* wName = Utf8ToWChar(pkt.userName);
				WCHAR* wRoom = Utf8ToWChar(pkt.roomName);
				wcout << L"User: " << wName << L" Enter Room: " << wRoom << endl;

				header = MakeHeader<PacketRoomFullBroad>(PacketId::Room_Full_Broad);
				temp->WritePacketHeader(header);
				temp->Write(pkt.roomName, MAX_USER_NAME_SIZE);
				GSessionManager.Broadcast(temp);

				PacketHeader header = MakeHeader<PacketRoomEnterRes>(PacketId::Room_Enter_Res);
				GameSessionRef s1 = static_pointer_cast<GameSession>(room->_session1);
				outStream->WriteRoomEnterRes(header, 0, s1->_name, _name, pkt.roomName);
				//Send(outStream);
				//s1->Send(outStream);
				_room->SendEnterUser(outStream);
				my = 2;
			}
		}
		if (!isExist) {
			RoomRef room = make_shared<Room>();
			room->_session1 = shared_from_this();
			this->_room = room;

			WCHAR* wName = Utf8ToWChar(pkt.userName);
			WCHAR* wRoom = Utf8ToWChar(pkt.roomName);
			wcout << L"User: " << wName << L" Create Room: " << wRoom << endl;

			::memcpy(room->_name, pkt.roomName, MAX_USER_NAME_SIZE);
			GRoomManager.Add(room);
			_game = true;
			header = MakeHeader<PacketRoomOpenBroad>(PacketId::Room_Open_Broad);
			temp->WritePacketHeader(header);
			temp->Write(pkt.roomName, MAX_USER_NAME_SIZE);
			for (auto session : GSessionManager._sessions) {
				if (session->_game == false)
					session->Send(temp);
			}
			PacketHeader header = MakeHeader<PacketRoomEnterRes>(PacketId::Room_Enter_Res);
			BYTE user2[MAX_USER_NAME_SIZE] = "";
			outStream->WriteRoomEnterRes(header, 0, _name, user2, pkt.roomName);
			Send(outStream);
			my = 1;
		}
	}
	else if (id == PacketId::Game_Pos_Req) {
		PacketGamePosReq pkt;
		inStream->Read(pkt.x);
		inStream->Read(pkt.y);

		PacketHeader header = MakeHeader<PacketGamePosRes>(PacketId::Game_Pos_Res);
		outStream->WritePacketHeader(header);
		outStream->Write((uint32)0);
		outStream->Write(pkt.x);
		outStream->Write(pkt.y);
		wcout << "User: " << Utf8ToWChar(_name);
		cout << " Coordinate (" << pkt.x << ", " << pkt.y << ")" << endl;
		_room->SendEnterUser(outStream);
		uint16 win = _room->AddStone(_name, 18-pkt.y, pkt.x);
		
		// make PacketGameOutcomeBroad
		header = MakeHeader<PacketGameOutcomeBroad>(PacketId::Game_Outcome_Broad);
		temp->WritePacketHeader(header);
		temp->Write(win);
		_room->SendEnterUser(temp);

		//if (win != 0) {
		//	GameSessionRef s1 = static_pointer_cast<GameSession>(_room->_session1);
		//	GameSessionRef s2 = static_pointer_cast<GameSession>(_room->_session2);
		//	s1->_room = nullptr;
		//	s2->_room = nullptr;
		//	GRoomManager.Remove(_room);
		//	_room = nullptr;
		//}
		//if (win == 1) 
		//	wcout << L"Winner: " << Utf8ToWChar(s1->_name) << endl;
		//else if (win == 2)
		//	wcout << L"Winner: " << Utf8ToWChar(s2->_name) << endl;
	}
	else if (id == PacketId::Room_Leave_Req) {
		//1. user1일 때
		if (my == 1)
			static_pointer_cast<GameSession>(_room->_session1) = nullptr;
		//2. user2일 때
		else if(my == 2)
			static_pointer_cast<GameSession>(_room->_session2) = nullptr;

		if (_room->_session1 == nullptr && _room->_session2 == nullptr){
			BYTE roomName[MAX_USER_NAME_SIZE];
			::memcpy(roomName, _room->_name, MAX_USER_NAME_SIZE);
			PacketHeader header = MakeHeader<PacketRoomLeaveRes>(PacketId::Room_Leave_Res);
			outStream->WritePacketHeader(header);
			outStream->Write(roomName, MAX_USER_NAME_SIZE);
			GSessionManager.Broadcast(outStream);
			wcout << L"Remove Room:" << Utf8ToWChar(_room->_name) << endl;
			GRoomManager.Remove(_room);
		}
		_room = nullptr;
		my = 0;
		_game = false;
	}
}

