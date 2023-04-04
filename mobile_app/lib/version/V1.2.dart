import 'package:flutter/material.dart';
int timerTidur = 0;
int timerStandby = 0;

void main() {
  runApp( MaterialApp(
    home: Home()
    ),);
}

class Home extends StatelessWidget {
  const Home({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {

    return Scaffold(
      appBar: AppBar(
        toolbarHeight: 36,
        title: Text(
          'SMART SLEEP DETECTOR',
          style: TextStyle(
            fontSize: 16.0,
            color: Colors.white,
            fontFamily: 'Poppins-SemiBold',
          ),
        ),
        centerTitle: true,
        backgroundColor: Color(0xff47454B),
        shape: const RoundedRectangleBorder(
          borderRadius: BorderRadius.only(
            bottomLeft: Radius.circular(15),
            bottomRight: Radius.circular(15),
          ),
        ),
      ),
      body: Column(


        children: [
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
            child: Text(
              'Hallo, User',
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
          SizedBox(height: 10),

          // FOTO KONDISI
          Container(
            width: 150,
            height: 150,
            decoration: BoxDecoration(
              image: DecorationImage(
                image: AssetImage('images/awake.jpeg'),
                fit: BoxFit.cover,
              ),
            ),
          ),
          SizedBox(height: 10),

          // STATUS USER
          Container(
            width: 300,
            height: 40,
            decoration: BoxDecoration(
              borderRadius: BorderRadius.circular(15),
              color: Color(0xffFDB004),
            ),
            child: Center(
              child: Text(
                '< USER TERJAGA >',
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
            ),
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
                      margin: EdgeInsets.all(13),
                      height: 50,
                      decoration: BoxDecoration(
                        color: Color(0xffE8DABB),
                        borderRadius: BorderRadius.circular(15),
                      ),
                      child: Center(
                        child: TextField(
                          keyboardType: TextInputType.number,
                          decoration: InputDecoration(
                            hintText: 'Waktu (ms)',
                            border: InputBorder.none,
                            contentPadding: EdgeInsets.symmetric(horizontal: 10),
                          ),
                          onChanged: (value) {
                            timerTidur = int.parse(value);
                          },
                        ),
                      ),
                    ),
                  ),
                  Expanded(
                    child: Container(
                      margin: EdgeInsets.all(13),
                      height: 50,
                      decoration: BoxDecoration(
                        color: Color(0xffE8DABB),
                        borderRadius: BorderRadius.circular(15),
                      ),
                      child: Center(
                        child: TextField(
                          keyboardType: TextInputType.number,
                          decoration: InputDecoration(
                            hintText: 'Waktu (ms)',

                            border: InputBorder.none,
                            contentPadding: EdgeInsets.symmetric(horizontal: 10),
                          ),
                          onChanged: (value) {
                            timerStandby = int.parse(value);

                          },
                        ),
                      ),
                    ),
                  ),
                ],
              ), // Row 2
            ],
          ),
          SizedBox(height: 10),

          // PILIHAN MODE
          Container(
            height: 50,
            margin: EdgeInsets.all(0),
            child: Row(
              children: [
                Expanded(
                  flex: 2,
                  child: Container(
                    child: Center(
                      child: Text(
                        'Pilih Mode:',
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
                        onChanged: (value) {},
                        hint: Text(
                          'Pilih Mode',
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

          Container(
            width: 300,
            height: 40,
            margin: EdgeInsets.only(left: 10, right: 200), // memberikan margin pada sisi kanan
            decoration: BoxDecoration(
              borderRadius: BorderRadius.circular(15),
              color: Color(0xffFDB004),
            ),
            child: Align(
              alignment: Alignment.centerLeft,
              child: Center(
                child: Text(
                  'Log Aktivitas',
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
              ),
            ),
          ),
          SizedBox(height: 10),

          Container(
            width: 400,
            height: 170,
            decoration: BoxDecoration(
              borderRadius: BorderRadius.circular(15),
              color: Colors.grey[200],
            ),
            child: SingleChildScrollView(
              child: Padding(
                padding: EdgeInsets.all(10),
                child: Text(
                  'Institut Teknologi Bandung adalah sebuah perguruan tinggi\n negeri yang berkedudukan di Kota Bandung. Nama ITB diresmikan pada tanggal 2\n Maret 1959. Sejak tanggal 14 Oktober 2013 ITB menjadi Perguruan Tinggi\n Negeri Badan Hukum yang memiliki otonomi pengelolaan dalam akademik dan nonakademik.',
                  style: TextStyle(
                    fontSize: 17,
                    fontWeight: FontWeight.bold,
                    fontFamily: 'Poppins-SemiBold',
                    color: Colors.black,
                  ),
                ),
              ),
            ),
          ),


        ],
      )
      );
  }
}

