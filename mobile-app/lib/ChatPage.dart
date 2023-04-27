import 'dart:async';
import 'dart:convert';
import 'dart:typed_data';

import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

// Inisiasi variabel
int timerTidur = 0;
String sendtimerTidur = "";
int timerStandby = 0;
String sendtimerStandby = "";
String dropdownValue = 'Pilih Mode';
String selectedMenu = "Pilih Mode";

class ChatPage extends StatefulWidget {

  final BluetoothDevice server;

  const ChatPage({required this.server});

  @override
  _ChatPage createState() => new _ChatPage();
}

class _Message {
  int whom;
  String text;

  _Message(this.whom, this.text);
}

class _ChatPage extends State<ChatPage> {
  static final clientID = 0;
  BluetoothConnection? connection;

  List<_Message> messages = List<_Message>.empty(growable: true);
  String _messageBuffer = '';
  String dataString = '';

  final TextEditingController textEditingController =
      new TextEditingController();
  final ScrollController listScrollController = new ScrollController();

  bool isConnecting = true;
  bool get isConnected => (connection?.isConnected ?? false);

  bool isDisconnecting = false;

  @override
  void initState() {
    super.initState();

    BluetoothConnection.toAddress(widget.server.address).then((_connection) {
      print('Connected to the device');
      connection = _connection;
      setState(() {
        isConnecting = false;
        isDisconnecting = false;
      });

      connection!.input!.listen(_onDataReceived).onDone(() {
        // Example: Detect which side closed the connection
        // There should be `isDisconnecting` flag to show are we are (locally)
        // in middle of disconnecting process, should be set before calling
        // `dispose`, `finish` or `close`, which all causes to disconnect.
        // If we except the disconnection, `onDone` should be fired as result.
        // If we didn't except this (no flag set), it means closing by remote.
        if (isDisconnecting) {
          print('Disconnecting locally!');
        } else {
          print('Disconnected remotely!');
        }
        if (this.mounted) {
          setState(() {});
        }
      });
    }).catchError((error) {
      print('Cannot connect, exception occured');
      print(error);
    });
  }

  @override
  void dispose() {
    // Avoid memory leak (`setState` after dispose) and disconnect
    if (isConnected) {
      isDisconnecting = true;
      connection?.dispose();
      connection = null;
    }

    super.dispose();
  }


  @override
  Widget build(BuildContext context) {
    final List<Row> list = messages.map((_message) {
      return Row(
        children: <Widget>[
          Container(
            child: Text(
                (text) {
                  return text == '/shrug' ? '¯\\_(ツ)_/¯' : text;
                }(_message.text.trim()),
                style: TextStyle(color:
                _message.whom == clientID ? Colors.black87 : Colors.white,)),
            padding: EdgeInsets.all(12.0),
            margin: EdgeInsets.only(bottom: 8.0, left: 8.0, right: 8.0),
            width: 222.0,
            decoration: BoxDecoration(
                color:
                    _message.whom == clientID ? Color(0xffFFD882) : Colors.grey,
                borderRadius: BorderRadius.circular(7.0)),
          ),
        ],
        mainAxisAlignment: _message.whom == clientID
            ? MainAxisAlignment.end
            : MainAxisAlignment.start,
      );
    }).toList();

    String cleanMessageBuffer = _messageBuffer.trim().replaceAll('\n', '').replaceAll('\r', '');

    final serverName = widget.server.name ?? "Unknown";
    return Scaffold(
      resizeToAvoidBottomInset: false,
      backgroundColor: Colors.white,
      appBar: AppBar(
        toolbarHeight: 36,
        title: isConnected
            ? Text('SMART SLEEP DETECTOR', style: TextStyle(
          fontSize: 16.0,
          color: Colors.white,
          fontFamily: 'Poppins-SemiBold',
        ),)
            : Text(
          'WAITING',
          style: TextStyle(
            fontSize: 16.0,
            color: Colors.white,
            fontFamily: 'Poppins-SemiBold',
          ),
        ),
        centerTitle: true,
        backgroundColor: isConnected ? Color(0xff47454B) : Colors.orange,
        shape: const RoundedRectangleBorder(
          borderRadius: BorderRadius.only(
            bottomLeft: Radius.circular(15),
            bottomRight: Radius.circular(15),
          ),
        ),
      ),

      body: SafeArea(
        child: Column(
          children: <Widget>[
            SizedBox(height: 10),

            // SAPAAN, "Halo User"
            Container(
              width: 200,
              height: 40,
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(15),
                color: Color(0xffFDB004),
              ),
              child: Center(
                child: Column(
                  children: <Widget>[
                    Expanded(
                      flex: 1,
                      child: Container(
                        alignment: Alignment.center,
                        child: isConnected
                            ? Text('Terhubung : ' + serverName,
                            style: TextStyle(
                              fontSize: 17,
                              fontWeight: FontWeight.bold,
                              fontFamily: 'Poppins-SemiBold',
                              color: Colors.white,
                              shadows: [
                                Shadow(
                                  color: Colors.black.withOpacity(0.3), // warna bayangan
                                  offset: Offset(0, 2), // posisi bayangan
                                  blurRadius: 8, // ukuran blur bayangan
                                ),
                              ],
                            ))
                            : Text(
                          'Menunggu Koneksi',
                          style: TextStyle(
                            fontSize: 18,
                            fontWeight: FontWeight.bold,
                            fontFamily: 'Poppins-SemiBold',
                            color: Colors.white,
                            shadows: [
                              Shadow(
                                color: Colors.black.withOpacity(0.3), // warna bayangan
                                offset: Offset(0, 2), // posisi bayangan
                                blurRadius: 8, // ukuran blur bayangan
                              ),
                            ],
                          ),
                        ),
                      ),
                    ),
                  ],
                ),
              ),
            ),

            SizedBox(height: 10),

            // FOTO KONDISI
            Container(
              width: 150,
              height: 150,
              decoration: BoxDecoration(
                image: DecorationImage(
                  image: AssetImage(
                    cleanMessageBuffer == "Received data: K0"
                        ? 'images/fit.jpg'
                        : cleanMessageBuffer == "Received data: K1"
                        ? 'images/sleep.jpeg'
                        : cleanMessageBuffer == "Received data: K2"
                        ? 'images/sleep.jpeg'

                        : 'images/clock.jpg', // fallback jika nilai tidak sesuai
                  ),
                  fit: BoxFit.cover,
                ),
              ),
            ),
            SizedBox(height: 10),

            // STATUS USER
            Container(
              width: 295,
              height: 40,
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(15),
                color: Color(0xffFDB004),
              ),
                child: Center(
                  child: Text(
                    cleanMessageBuffer == "Received data: K0"
                        ? "< USER TERJAGA >"
                        : cleanMessageBuffer == "Received data: K1"
                        ? "< USER TIDUR >"
                        : cleanMessageBuffer == "Received data: K2"
                        ? "< USER IDLE >"
                        : "Standby",
                    style: TextStyle(
                      fontSize: 17,
                      fontWeight: FontWeight.bold,
                      fontFamily: 'Poppins-SemiBold',
                      color: Colors.white,
                      shadows: [
                        Shadow(
                          color: Colors.black.withOpacity(0.3), // warna bayangan
                          offset: Offset(0, 2), // posisi bayangan
                          blurRadius: 8, // ukuran blur bayangan
                        ),
                      ],
                    ),
                  ),
                )
            ),
            SizedBox(height: 10),

            // SETTING TIMER
            Column(
              children: [
                Row(
                  children: [
                    Expanded(
                      child: Container(
                        height: 40,
                        margin: EdgeInsetsDirectional.fromSTEB(35, 5, 10, 5),
                        child: Center(
                          child: Text(
                            'Timer Tidur',
                            style: TextStyle(
                              fontSize: 20,
                              fontFamily: 'Poppins-SemiBold',
                              color: Colors.black,
                            ),
                          ),
                        ),
                      ),
                    ),
                    Expanded(
                      child: Container(
                        height: 40,
                        margin: EdgeInsetsDirectional.fromSTEB(10, 5, 35, 5),
                        child: Center(
                          child: Text(
                            'Timer Standby',
                            style: TextStyle(
                              fontSize: 20,
                            ),
                          ),
                        ),
                      ),
                    ),
                  ],
                ), // Row 1
                Row(
                  children: [
                    Expanded(
                      child: Container(
                        margin: EdgeInsetsDirectional.fromSTEB(35, 0, 5, 0),
                        height: 50,
                        decoration: BoxDecoration(
                          color: Color(0xffE8DABE),
                          borderRadius: BorderRadius.circular(15),
                        ),
                        child: Row(
                          children: [
                            Expanded(
                              child: TextField(
                                keyboardType: TextInputType.number,
                                decoration: InputDecoration(
                                  hintText: '5 (s)',
                                  border: InputBorder.none,
                                  contentPadding: EdgeInsets.symmetric(horizontal: 10),
                                ),
                                  onChanged: (value) {
                                    timerTidur = int.parse(value);
                                    sendtimerTidur = "T" + timerTidur.toString();
                                  },
                              ),
                            ),
                            IconButton(
                              icon: Icon(Icons.send),
                              onPressed: () {
                                _sendMessage(sendtimerTidur);
                                _sendMessage(isConnected.toString());
                              },
                            ),
                          ],
                        ),

                      ),
                    ),
                    Expanded(
                      child: Container(
                        margin: EdgeInsetsDirectional.fromSTEB(5, 0, 35, 0),
                        height: 50,
                        decoration: BoxDecoration(
                          color: Color(0xffE8DABE),
                          borderRadius: BorderRadius.circular(15),
                        ),

                        child: Row(
                          children: [
                            Expanded(
                              child: TextField(
                                keyboardType: TextInputType.number,
                                decoration: InputDecoration(
                                  hintText: '10 (s)',
                                  border: InputBorder.none,
                                  contentPadding: EdgeInsets.symmetric(horizontal: 10),
                                ),
                                onChanged: (value) {
                                  timerStandby = int.parse(value);
                                  sendtimerStandby = "S" + timerStandby.toString();
                                },
                              ),
                            ),
                            IconButton(
                              icon: Icon(Icons.send),
                              onPressed: () {
                                _sendMessage(sendtimerStandby);
                              },
                            ),
                          ],
                        ),

                      )

                    ),
                  ],
                ), // Row 2
              ],
            ),
            SizedBox(height: 10),

            // PILIHAN MODE
            Container(
              height: 50,
              margin: EdgeInsetsDirectional.fromSTEB(20, 5, 30, 5),
              child: Row(
                children: [
                  Expanded(
                    flex: 2,
                    child: Container(
                      child: Center(
                        child: Text(
                          dropdownValue,
                          style: TextStyle(
                            fontSize: 18,
                            fontFamily: 'fonts/Poppins-Regular.ttf',
                            color: Colors.black,
                          ),
                        ),
                      ),
                    ),
                  ),
                  Expanded(
                    flex: 3,
                    child: Container(
                      margin: EdgeInsets.all(6),
                      decoration: BoxDecoration(
                        borderRadius: BorderRadius.circular(15),
                        color: Color(0xffFFD882),
                      ),
                      child: Center(
                        child: DropdownButton<String>(
                          items: [
                            DropdownMenuItem<String>(
                              value: 'mode1',
                              child: Text(
                                'Mode 1',
                                style: TextStyle(
                                  fontSize: 18,
                                  fontFamily: 'Poppins-SemiBold',
                                ),
                              ),
                            ),
                            DropdownMenuItem<String>(
                              value: 'mode2',
                              child: Text(
                                'Mode 2',
                                style: TextStyle(
                                  fontSize: 18,
                                  fontFamily: 'Poppins-SemiBold',
                                ),
                              ),
                            ),
                            DropdownMenuItem<String>(
                              value: 'mode3',
                              child: Text(
                                'Mode 3',
                                style: TextStyle(
                                  fontSize: 18,
                                  fontFamily: 'Poppins-SemiBold',
                                ),
                              ),
                            ),
                            DropdownMenuItem<String>(
                              value: 'mode4',
                              child: Text(
                                'Mode 4',
                                style: TextStyle(
                                  fontSize: 18,
                                  fontFamily: 'Poppins-SemiBold',
                                ),
                              ),
                            ),
                          ],
                          onChanged: (String? value) {
                            int menuNumber;
                            switch (value) {
                              case 'mode1':
                                menuNumber = 1;
                                break;
                              case 'mode2':
                                menuNumber = 2;
                                break;
                              case 'mode3':
                                menuNumber = 3;
                                break;
                              case 'mode4':
                                menuNumber = 4;
                                break;
                              default:
                                menuNumber = 0;
                                break;
                            }
                            if (menuNumber > 0) {
                              String message = "M" + menuNumber.toString();
                              _sendMessage(message);
                              String menuText = 'Mode ' + menuNumber.toString();
                              setState(() {
                                selectedMenu = menuText;
                              });
                            }
                          },
                          hint: Text(
                            selectedMenu,
                            style: TextStyle(
                              fontSize: 18,
                              fontFamily: 'Poppins-SemiBold',
                            ),
                          ),
                        ),
                      ),

                    ),
                  ),
                ],
              ),
            ),

            SizedBox(height: 10),

            Expanded(
              flex: 3,
              child: ListView(
                padding: const EdgeInsets.all(12.0),
                controller: listScrollController,
                children: list,
              ),
            ),
            Row(
              children: <Widget>[
                Expanded(
                  child: Container(
                    margin: const EdgeInsets.only(left: 16.0),
                    child: TextField(
                      style: const TextStyle(fontSize: 15.0),
                      controller: textEditingController,
                      decoration: InputDecoration.collapsed(
                        hintText: isConnecting
                            ? 'Wait until connected...'
                            : isConnected
                            ? 'Type your message...'
                            : 'Chat got disconnected',
                        hintStyle: const TextStyle(color: Colors.grey),
                      ),
                      enabled: isConnected,
                    ),
                  ),
                ),
                Container(
                  margin: const EdgeInsets.all(8.0),
                  child: IconButton(
                      icon: const Icon(Icons.send),
                      onPressed: isConnected
                          ? () => _sendMessage(textEditingController.text)
                          : null),
                ),
              ],
            )
          ],
        ),

      ),


    );
  }


  void _onDataReceived(Uint8List data) {
    // Allocate buffer for parsed data

    int backspacesCounter = 0;
    data.forEach((byte) {
      if (byte == 8 || byte == 127) {
        backspacesCounter++;
      }
    });
    Uint8List buffer = Uint8List(data.length - backspacesCounter);
    int bufferIndex = buffer.length;

    // Apply backspace control character
    backspacesCounter = 0;
    for (int i = data.length - 1; i >= 0; i--) {
      if (data[i] == 8 || data[i] == 127) {
        backspacesCounter++;
      } else {
        if (backspacesCounter > 0) {
          backspacesCounter--;
        } else {
          buffer[--bufferIndex] = data[i];
        }
      }
    }

    // Create message if there is new line character
    String dataString = String.fromCharCodes(buffer);
    int index = buffer.indexOf(13);
    if (~index != 0) {
      setState(() {
        messages.add(
          _Message(
            1,
            backspacesCounter > 0
                ? _messageBuffer.substring(
                    0, _messageBuffer.length - backspacesCounter)
                : _messageBuffer + dataString.substring(0, index),
          ),
        );
        _messageBuffer = dataString.substring(index);
      });
    } else {
      _messageBuffer = (backspacesCounter > 0
          ? _messageBuffer.substring(
              0, _messageBuffer.length - backspacesCounter)
          : _messageBuffer + dataString);
    }
    Future.delayed(Duration(milliseconds: 333)).then((_) {
      listScrollController.animateTo(
          listScrollController.position.maxScrollExtent,
          duration: Duration(milliseconds: 333),
          curve: Curves.easeOut);
    });

  }

  void _sendMessage(String text) async {
    text = text.trim();
    textEditingController.clear();

    if (text.length > 0) {
      try {
        connection!.output.add(Uint8List.fromList(utf8.encode(text + "\r\n")));
        await connection!.output.allSent;

        setState(() {
          messages.add(_Message(clientID, text));
        });

        Future.delayed(Duration(milliseconds: 333)).then((_) {
          listScrollController.animateTo(
              listScrollController.position.maxScrollExtent,
              duration: Duration(milliseconds: 333),
              curve: Curves.easeOut);
        });
      } catch (e) {
        // Ignore error, but notify state
        setState(() {});
      }
    }
  }
}
