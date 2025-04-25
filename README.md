# Autonomic_Robot_Wall-E

POLISH
Projekt wykonano w ramach studiów na Politechnice Łódzkiej na kierunku elektronika i telekomunikacja. Projekt ten polegal na zaprojektowaniu, zbudowaniu i zaprogramowaniu autonomicznego pojazdu elektronicznego. Pojazd autonomiczny oznacza, że nie steruje nim zdalnie człowiek w żaden sposób, jedynie ma kierować się w kierunku celu na podstawie napisanego kodu programu. Zespół projektowy składał się z 5 osób, z czego ja wykonałem około 70% pracy, ze względu na to, że większość grupy nie miała zbytnio czasu. Jedna osoba faktycznie wykonała powierzone zadania w 100%, druga w 50%, a pozostała dwójka osób niestety nie wykazała się znaczącą pracą. 

Założenia projektu:
•	Pojazd musi poruszać się samodzielnie, bez żadnego zdalnego sterowania
•	Pojazd musi być zdolny do zapobiegania przypadkowego wprowadzenia na przeszkodę.
•	Jeśli dojdzie do wjechania w przeszkodę pojazd wraca na początek toru i rozpoczyna przejazd ponownie.
•	Powierzchnia, po której będzie poruszał się pojazd to płaski obszar około 12 x 20 płytek (płytka 33 x 33 cm). Całość będzie ograniczona taśmą aluminiową naklejoną na powierzchnię. Podobnie jak uderzenie w przeszkodę, tak też opuszczenie ograniczonego terenu skutkuje powrotem na start i ponownym rozpoczęciem przejazdu.
•	Pojazd ma za zadanie ominąć wszystkie - minimum 3 przeszkody w postaci kartonowych ścian (10 cm x 20 cm, ściany w jasnych kolorach)
•	Czas przejazdu nie może przekraczać 5 min.
•	Linia mety będzie oznaczona taśmą magnetyczną.
•	System musi rejestrować całkowity czas przejazdu. Dane muszą być przesłane po przejechaniu trasy na urządzenie zewnętrzne i przedstawione zespołowi oceniającemu.
•	Estetyka wykonania pojazdu będzie również oceniana.
•	Wybór platformy mikroprocesorowej jest dowolny.
•	Koszt pojazdu nie powinien przekroczyć 700 zł - 350 zł dofinansowane przez uczelnię oraz 350 zł wkład własny.
•	Pojazd nie może mieć wystających części poza obrys podwozia.

Wykorzystane elementy w projekcie:
•	1x Podwozie robota Black Gladiator
•	2x Akumulator Samsung INR18650-30Q 3000mAh
•	1x Mikrokontroler STM32 NUCLEO-L476RG
•	1x Czujnik ultradźwiękowy HC-SR04
•	1x Sterownik silników Cytron Maker Drive MX1508
•	1x Czujnik pola magnetycznego Halla DFRobot Gravity
•	Przewody połączeniowe żeńsko-męskie justPi 20 cm
•	Przewody połączeniowe żeńsko-żeńskie justPi 20 cm
•	Goldpiny
•	Kabel USB-MiniUSB do STM32
•	Przedłużacz MiniUSB-MiniUSB 20 cm
•	Przełącznik ON/OFF
•	Taśma malarska

1. Do wykrywania przeszkód (tekturowych pudeł) wykorzystaliśmy cyfrowe czujniki odbicia podczerwieni oraz jeden czujnik ultradźwiękowy HC-SR04.
2. Do wykrywania granicy obszaru (taśmy aluminiowej) również zdecydowaliśmy się na wybór cyfrowych czujników odbicia podczerwieni. Chcieliśmy wykorzystać analogowe czujniki odbicia podczerwieni, jednak był problem z zaprogramowaniem ich na STM32, za to z cyfrowymi nie było żadnego problemu.
3. Do wykrywania mety (taśmy magnetycznej) zdecydowaliśmy się na czujnik Halla DF Robot Gravity, gdyż nie było lepszej alternatywy, w podobnej cenie. Musial być umieszczony jedynie kilka milimetrów nad ziemią, aby był w stanie wykryć taśmę magnetyczną. 
4. Do rejestracji czasu przejazdu wykorzystaliśmy zegar RTC mikrokontrolera.

Co zrobiłem w projekcie?
•	Wspólnie z kolegą i koleżanką zaprojektowaliśmy wygląd naszego robota.
•	Testowałem czujniki do wykrywania przeszkód, taśmy aluminiowej oraz magnetycznej w celu wyboru najlepszego czujnika.
•	Wykonałem obudowę robota, dzięki czemu była możliwość zamontowania mikrokontrolera, czujników i sterownika silników.
•	Wykonałem odpowiednie otwory mocujące w obudowie, zamontowałem elementy, połączyłem je przewodami ze sobą, stworzyłem płytkę z goldpinami do rozciągniecia zasilania 5V.
•	Za pomocą programu STM32 Cube IDE napisałem program, obsługujący wszystkie czujniki, aby pojazd był w stanie odpowiednio reagować na przeszkody, nie wjeżdzać na nie, nie wyjeżdzać poza ograniczony obszar oraz wykrywać metę.
•	Byłem odpowiedzialny za estetykę i wygląd robota.

Mimo niezbyt dobrego zgrania zespołu pojazd zajął drugie miejsce na 6! Świetnie wykrywał przeszkody, nie wyjechał poza ograniczony obszar oraz rejestrował czas przejazdu! Niestety nie dojechał do mety ze względu na rozładowanie się akumulatora i braku zapasowego (ograniczony budżet)



ENGLISH
The project was carried out as part of studies at the Lodz University of Technology in the field of electronics and telecommunications. This project consisted of designing, building and programming an autonomous electronic vehicle. An autonomous vehicle means that it is not remotely controlled by a human in any way, it only steers towards a destination based on written program code. The project team consisted of 5 people, of which I did about 70% of the work, due to the fact that most of the group did not have much time. One person actually completed the assigned tasks 100%, the other 50%, and the remaining two people unfortunately did not show any significant work.

Project assumptions:
• The vehicle must move independently, without any remote control
• The vehicle must be able to prevent accidental entry into an obstacle.
• If it runs into an obstacle, the vehicle returns to the beginning of the track and starts the ride again.
• The surface on which the vehicle will move is a flat area of ​​about 12 x 20 tiles (tile 33 x 33 cm). The whole will be limited by aluminum tape stuck to the surface. Similarly to hitting an obstacle, leaving the restricted area results in returning to the start and starting the ride again.
• The vehicle must avoid all - at least 3 obstacles in the form of cardboard walls (10 cm x 20 cm, walls in light colors)
• The ride time cannot exceed 5 minutes
• The finish line will be marked with magnetic tape
• The system must record the total ride time. Data must be sent to an external device after the route has been completed and presented to the evaluation team
• The aesthetics of the vehicle will also be assessed
• The choice of microprocessor platform is optional
• The cost of the vehicle should not exceed PLN 700 - PLN 350 co-financed by the university and PLN 350 own contribution
• The vehicle cannot have any parts protruding beyond the outline of the chassis

Elements used in the project:
• 1x Black Gladiator Robot Chassis
• 2x Samsung INR18650-30Q 3000mAh Battery
• 1x STM32 NUCLEO-L476RG Microcontroller
• 1x HC-SR04 Ultrasonic Sensor
• 1x Cytron Maker Drive MX1508 Motor Controller
• 1x DFRobot Gravity Hall Magnetic Field Sensor
• justPi 20 cm Female-Male Connecting Cables
• justPi 20 cm Female-Female Connecting Cables
• Goldpins
• USB-MiniUSB Cable for STM32
• MiniUSB-MiniUSB Extension Cable 20 cm
• ON/OFF Switch
• Masking Tape

1. To detect obstacles (cardboard boxes) we used digital infrared reflection sensors and one HC-SR04 ultrasonic sensor

2. We also decided to choose digital infrared reflection sensors to detect the boundary of the area (aluminum tape). We wanted to use analog infrared reflection sensors, but there was a problem with programming them on STM32, but there was no problem with digital ones.
3. To detect the finish line (magnetic tape), we decided on the DF Robot Gravity Hall sensor, because there was no better alternative at a similar price. It had to be placed only a few millimeters above the ground to be able to detect the magnetic tape.
4. We used the RTC clock of the microcontroller to record the travel time.

What did I do in the project?
• Together with a colleague, we designed the appearance of our robot.
• I tested sensors for detecting obstacles, aluminum tape and magnetic tape in order to choose the best sensor.
• I made the robot's housing, thanks to which it was possible to mount a microcontroller, sensors and a motor controller.
• I made the appropriate mounting holes in the housing, mounted the elements, connected them with wires, created a board with goldpins to extend the 5V power supply.
• Using the STM32 Cube IDE program, I wrote a program that supports all sensors so that the vehicle could react appropriately to obstacles, not drive into them, not drive outside the restricted area and detect the finish line.
• I was responsible for the aesthetics and appearance of the robot.

Despite the team's not very good cooperation, the vehicle took second place out of 6! It detected obstacles perfectly, did not drive outside the restricted area and recorded the travel time! Unfortunately, it did not reach the finish line due to a flat battery and lack of a spare (limited budget)
