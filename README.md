# Autonomic_Robot_Wall-e

POLISH
Projekt wykonano w ramach studiów na Politechnice Łódzkiej na kierunku elektronika i telekomunikacja. Projekt ten polegal na zaprojektowaniu, zbudowaniu i zaprogramowaniu autonomicznego pojazdu elektronicznego. Zespół projektowy składał się z 5 osób, z czego ja wykonałem około 70% pracy, ze względu na to, że większość grupy nie miała czasu ani chęci się zebrać. Jedna osoba faktycznie wykonała powierzone zadania w 100%, druga w 50%, a pozostała dwójka osób niestety nie wykazała się znaczącą pracą. 

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

1. Do wykrywania przeszkód (tekturowych pudeł) wykorzystaliśmy cyfrowe czujniki odbicia podczerwieni oraz jeden czujnik ultradźwiękowy HC-SR04.
2. Do wykrywania granicy obszaru (taśmy aluminiowej) również zdecydowaliśmy się na wybór cyfrowych czujników odbicia podczerwieni. Chcieliśmy wykorzystać analogowe czujniki odbicia podczerwieni, jednak był problem z zaprogramowaniem ich na STM32, za to z cyfrowymi nie było żadnego problemu.
3. Do wykrywania mety (taśmy magnetycznej) zdecydowaliśmy się na czujnik Halla DF Robot Gravity, gdyż nie było lepszej alternatywy, w podobnej cenie. Musial być umieszczony jedynie kilka milimetrów nad ziemią, aby był w stanie wykryć taśmę magnetyczną. 
4. Do rejestracji czasu przejazdu wykorzystaliśmy 

Co zrobiłem w projekcie?
•	Wspólnie z kolegą i koleżanką zaprojektowaliśmy wygląd naszego robota.
•	Testowałem czujniki do wykrywania przeszkód, taśmy aluminiowej oraz magnetycznej w celu wyboru najlepszego czujnika.
•	Wykonałem obudowę robota, dzięki czemu była możliwość zamontowania mikrokontrolera, czujników i sterownika silników.
•	Wykonałem odpowiednie otwory mocujące w oboudowei, zamontowałem elementy, połączyłem je przewodami ze sobą, stworzyłem płytkę z goldpinami do rozciągniecia zasilania 5V.
•	Za pomocą programu STM32 Cube IDE zaprogramowałem mikrokontroler STM32 Cube IDE, aby pojazd był w stanie odpowiednio reagować na przeszkody, nie wjeżdzać na nie, nie wyjeżdzać poza ograniczony obszar oraz wykrywać metę.
•	Byłem odpowiedzialny za estetykę i wygląd robota.





ENGLISH
This project was about designing, 
