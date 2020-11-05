//
//  Names.swift
//  Svátky
//
//  Created by Štěpán Vích on 03/05/2018.
//  Copyright © 2018 Štěpán Vích. All rights reserved.
//

import Foundation

public struct Day {
    let day:   Int;
    let month: Int;
    let names : [String];
    
    init(_ day: Int, _ month: Int, _ names : [String]) {
        self.day = day;
        self.month = month;
        self.names = names;
    }
    
    public func calculateOffset(dayToAdd: Int) -> Day {
        let dayDate = NSDateComponents()
        dayDate.month = self.month
        dayDate.day = self.day
        let notificationDate = NSCalendar(identifier: NSCalendar.Identifier.gregorian)?.date(from: dayDate as DateComponents)

        let offsetDate = Calendar.current.date(byAdding: .day, value: dayToAdd, to: notificationDate!);
        
        // Extract day, month and year from offsetDate
        let f = DateFormatter();
        f.dateFormat = "MM";
        let offsetMonth = Int(f.string(from: offsetDate!));
        f.dateFormat = "dd";
        let offsetDay = Int(f.string(from: offsetDate!));
        return Day(offsetDay!, offsetMonth!, self.names);
    }
    
}




// Help class for searching in static structure
public class Names {
    
    public static func nameToDate(name:String) -> Day? {
        for record in namesArray {
            for recordName in record.names {
                if recordName.lowercased() == name.lowercased() {
                    return Day(record.day, record.month, [name]);
                }
            }
        }
        return nil;
    }
    
    public static func dateToName(day: Int, month: Int) -> String {

        for record in namesArray {
            if record.day == day && record.month == month {
                return record.names[0];
            }
        }
        return "--";
    }
    
    private static let namesArray:[Day] = [
        Day(1,1, ["Den obnovy"]),
        Day(2,1, ["Karina", "Karína", "Karin"]),
        Day(3,1, ["Radmila"]),
        Day(4,1, ["Diana"]),
        Day(5,1, ["Dalimil", "Dalemil"]),
        Day(6,1, ["Tři Králové"]),
        Day(7,1, ["Vilma", "Wilhelmina"]),
        Day(8,1, ["Čestmír"]),
        Day(9,1, ["Vladan"]),
        Day(10,1, ["Břetislav"]),
        Day(11,1, ["Bohdana"]),
        Day(12,1, ["Pravoslav"]),
        Day(13,1, ["Edita"]),
        Day(14,1, ["Radovan"]),
        Day(15,1, ["Alice"]),
        Day(16,1, ["Ctirad", "Česlav"]),
        Day(17,1, ["Drahoslav"]),
        Day(18,1, ["Vladislav"]),
        Day(19,1, ["Doubravka"]),
        Day(20,1, ["Ilona"]),
        Day(21,1, ["Běla"]),
        Day(22,1, ["Slavomír"]),
        Day(23,1, ["Zdeněk", "Zdenek", "Zdenko"]),
        Day(24,1, ["Milena"]),
        Day(25,1, ["Miloš"]),
        Day(26,1, ["Zora"]),
        Day(27,1, ["Ingrid"]),
        Day(28,1, ["Otýlie", "Otilie"]),
        Day(29,1, ["Zdislava", "Zdeslava"]),
        Day(30,1, ["Robin"]),
        Day(31,1, ["Marika"]),
        
        Day(1,2, ["Hynek"]),
        Day(2,2, ["Nela"]),
        Day(3,2, ["Blažej"]),
        Day(4,2, ["Jarmila"]),
        Day(5,2, ["Dobromila"]),
        Day(6,2, ["Vanda", "Wanda"]),
        Day(7,2, ["Veronika"]),
        Day(8,2, ["Milada"]),
        Day(9,2, ["Apolena"]),
        Day(10,2, ["Mojmír"]),
        Day(11,2, ["Božena"]),
        Day(12,2, ["Slavěna"]),
        Day(13,2, ["Věnceslav"]),
        Day(14,2, ["Valentýn", "Valentin", "Valentýna"]),
        Day(15,2, ["Jiřina", "Jorga"]),
        Day(16,2, ["Ljuba"]),
        Day(17,2, ["Miloslav"]),
        Day(18,2, ["Gizela", "Gisela"]),
        Day(19,2, ["Patrik"]),
        Day(20,2, ["Oldřich"]),
        Day(21,2, ["Lenka", "Eleonora"]),
        Day(22,2, ["Petr"]),
        Day(23,2, ["Svatopluk"]),
        Day(24,2, ["Matěj", "Matej", "Matyáš"]),
        Day(25,2, ["Liliana", "Lilian", "Lily"]),
        Day(26,2, ["Dorota", "Dorotea"]),
        Day(27,2, ["Alexandr", "Alexander", "Alexis"]),
        Day(28,2, ["Lumír"]),
        
        Day(1,3, ["Bedřich"]),
        Day(2,3, ["Anežka", "Agnes", "Ines"]),
        Day(3,3, ["Kamil"]),
        Day(4,3, ["Stela", "Stella"]),
        Day(5,3, ["Kazimír"]),
        Day(6,3, ["Miroslav", "Mirek"]),
        Day(7,3, ["Tomáš", "Thomas", "Tom"]),
        Day(8,3, ["Gabriela"]),
        Day(9,3, ["Františka", "Francesca"]),
        Day(10,3, ["Viktorie"]),
        Day(11,3, ["Anděla", "Andělína", "Angelina"]),
        Day(12,3, ["Řehoř", "Gregor"]),
        Day(13,3, ["Růžena", "Rozálie", "Rosita"]),
        Day(14,3, ["Rút", "Rut", "Matylda"]),
        Day(15,3, ["Ida"]),
        Day(16,3, ["Elena", "Herbert"]),
        Day(17,3, ["Vlastimil"]),
        Day(18,3, ["Eduard", "Edvard"]),
        Day(19,3, ["Josef", "Jozef"]),
        Day(20,3, ["Světlana"]),
        Day(21,3, ["Radek", "Radomil"]),
        Day(22,3, ["Leona"]),
        Day(23,3, ["Ivona", "Yvona"]),
        Day(24,3, ["Gabriel"]),
        Day(25,3, ["Marián", "Marian", "Marius"]),
        Day(26,3, ["Emanuel"]),
        Day(27,3, ["Dita", "Ditta"]),
        Day(28,3, ["Soňa", "Sonja", "Sonia"]),
        Day(29,3, ["Taťana", "Tatiana"]),
        Day(30,3, ["Arnošt"]),
        Day(31,3, ["Kvido", "Quido"]),
        
        Day(1,4, ["Hugo"]),
        Day(2,4, ["Erika"]),
        Day(3,4, ["Richard"]),
        Day(4,4, ["Ivana", "Ivanka"]),
        Day(5,4, ["Miroslava", "Mirka"]),
        Day(6,4, ["Vendula", "Vendulka"]),
        Day(7,4, ["Heřman", "Herman", "Hermína"]),
        Day(8,4, ["Ema", "Emma"]),
        Day(9,4, ["Dušan", "Dušana"]),
        Day(10,4, ["Darja", "Daria", "Darya"]),
        Day(11,4, ["Izabela", "Isabel"]),
        Day(12,4, ["Julius", "Július", "Julian"]),
        Day(13,4, ["Aleš"]),
        Day(14,4, ["Vincenc", "Vincent"]),
        Day(15,4, ["Anastázie", "Anastazia"]),
        Day(16,4, ["Irena", "Irini"]),
        Day(17,4, ["Rudolf", "Rolf"]),
        Day(18,4, ["Valerie", "Valérie", "Valeria"]),
        Day(19,4, ["Rostislava"]),
        Day(20,4, ["Marcela"]),
        Day(21,4, ["Alexandra"]),
        Day(22,4, ["Evženie", "Evžénie"]),
        Day(23,4, ["Vojtěch"]),
        Day(24,4, ["Jiří", "Juraj", "George"]),
        Day(25,4, ["Marek"]),
        Day(26,4, ["Oto", "Ota", "Otto"]),
        Day(27,4, ["Jaroslav"]),
        Day(28,4, ["Vlastislav"]),
        Day(29,4, ["Robert"]),
        Day(30,4, ["Blahoslav"]),
        
        Day(1,5, ["Svátek Práce"]),
        Day(2,5, ["Zikmund"]),
        Day(3,5, ["Alexej", "Alex"]),
        Day(4,5, ["Květoslav"]),
        Day(5,5, ["Klaudie", "Klaudia", "Claudia"]),
        Day(6,5, ["Radoslav"]),
        Day(7,5, ["Stanislav"]),
        Day(8,5, ["Den Vítězství"]),
        Day(9,5, ["Ctibor", "Stibor"]),
        Day(10,5, ["Blažena"]),
        Day(11,5, ["Svatava"]),
        Day(12,5, ["Pankrác"]),
        Day(13,5, ["Servác"]),
        Day(14,5, ["Bonifác"]),
        Day(15,5, ["Žofie"]),
        Day(16,5, ["Přemysl"]),
        Day(17,5, ["Aneta", "Anitta"]),
        Day(18,5, ["Nataša"]),
        Day(19,5, ["Ivo", "Ivoš"]),
        Day(20,5, ["Zbyšek"]),
        Day(21,5, ["Monika"]),
        Day(22,5, ["Emil"]),
        Day(23,5, ["Vladimír"]),
        Day(24,5, ["Jana"]),
        Day(25,5, ["Viola"]),
        Day(26,5, ["Filip"]),
        Day(27,5, ["Valdemar", "Waldemar"]),
        Day(28,5, ["Vilém"]),
        Day(29,5, ["Maxmilián", "Maxmilian", "Maximilian"]),
        Day(30,5, ["Ferdinand"]),
        Day(31,5, ["Kamila"]),
        
        Day(1,6, ["Laura"]),
        Day(2,6, ["Jarmil"]),
        Day(3,6, ["Tamara"]),
        Day(4,6, ["Dalibor"]),
        Day(5,6, ["Dobroslav"]),
        Day(6,6, ["Norbert"]),
        Day(7,6, ["Iveta", "Yveta"]),
        Day(8,6, ["Medard"]),
        Day(9,6, ["Stanislava"]),
        Day(10,6, ["Otta"]),
        Day(11,6, ["Bruno"]),
        Day(12,6, ["Antonie", "Antonina"]),
        Day(13,6, ["Antonín", "Antonin"]),
        Day(14,6, ["Roland"]),
        Day(15,6, ["Vít", "Vítek"]),
        Day(16,6, ["Zbyněk"]),
        Day(17,6, ["Adolf"]),
        Day(18,6, ["Milan"]),
        Day(19,6, ["Leoš"]),
        Day(20,6, ["Květa", "Kveta"]),
        Day(21,6, ["Alois", "Aloys", "ALojz"]),
        Day(22,6, ["Pavla"]),
        Day(23,6, ["Zdeňka", "Zdena", "Zdenka"]),
        Day(24,6, ["Jan"]),
        Day(25,6, ["Ivan"]),
        Day(26,6, ["Adriana"]),
        Day(27,6, ["Ladislav"]),
        Day(28,6, ["Lubomír", "Lubomil"]),
        Day(29,6, ["Petr", "Pavel"]),
        Day(30,6, ["Šárka"]),
    
        Day(1,7, ["Jaroslava"]),
        Day(2,7, ["Patricie"]),
        Day(3,7, ["Radomír"]),
        Day(4,7, ["Prokop"]),
        Day(5,7, ["Den slovanských věrozvěstů Cyrila a Metoděje"]),
        Day(6,7, ["Den upáleni mistra Jana Husa"]),
        Day(7,7, ["Bohuslava"]),
        Day(8,7, ["Nora"]),
        Day(9,7, ["Drahoslava"]),
        Day(10,7, ["Libuše", "Amálie"]),
        Day(11,7, ["Olga"]),
        Day(12,7, ["Bořek"]),
        Day(13,7, ["Markéta", "Margareta", "Margit"]),
        Day(14,7, ["Karolína", "Karolina", "Karla"]),
        Day(15,7, ["Jindřich"]),
        Day(16,7, ["Luboš", "Liboslava", "Luboslav"]),
        Day(17,7, ["Martina"]),
        Day(18,7, ["Drahomíra"]),
        Day(19,7, ["Čeněk"]),
        Day(20,7, ["Ilja"]),
        Day(21,7, ["Vítězslav"]),
        Day(22,7, ["Magdaléna", "Magdalena"]),
        Day(23,7, ["Libor"]),
        Day(24,7, ["Kristýna", "Kristina", "Kristen"]),
        Day(25,7, ["Jakub"]),
        Day(26,7, ["Anna"]),
        Day(27,7, ["Věroslav"]),
        Day(28,7, ["Viktor", "Victor"]),
        Day(29,7, ["Marta", "Martha"]),
        Day(30,7, ["Bořivoj"]),
        Day(31,7, ["Ignác"]),
        
        Day(1,8, ["Oskar"]),
        Day(2,8, ["Gustav"]),
        Day(3,8, ["Miluše"]),
        Day(4,8, ["Dominik"]),
        Day(5,8, ["Kristián", "Milivoj", "Křišťan"]),
        Day(6,8, ["Oldřiška"]),
        Day(7,8, ["Lada"]),
        Day(8,8, ["Soběslav"]),
        Day(9,8, ["Roman"]),
        Day(10,8, ["Vavřinec"]),
        Day(11,8, ["Zuzana", "Susana"]),
        Day(12,8, ["Klára", "Clara"]),
        Day(13,8, ["Alena"]),
        Day(14,8, ["Alan"]),
        Day(15,8, ["Hana", "Hanka", "Hannah"]),
        Day(16,8, ["Jáchym", "Joachim"]),
        Day(17,8, ["Petra", "Petronila"]),
        Day(18,8, ["Helena"]),
        Day(19,8, ["Ludvík"]),
        Day(20,8, ["Bernard"]),
        Day(21,8, ["Johana"]),
        Day(22,8, ["Bohuslav", "Božislav"]),
        Day(23,8, ["Sandra"]),
        Day(24,8, ["Bartoloměj", "Bartolomej"]),
        Day(25,8, ["Radim", "Radimír"]),
        Day(26,8, ["Luděk"]),
        Day(27,8, ["Otakar", "Otokar"]),
        Day(28,8, ["Augustýn", "Augustin"]),
        Day(29,8, ["Evelína", "Evelin", "Evelina"]),
        Day(30,8, ["Vladěna"]),
        Day(31,8, ["Pavlína", "Paulína"]),
        

        
        Day(1,9, ["Linda", "Samuel"]),
        Day(2,9, ["Adéla", "Adléta", "Adela"]),
        Day(3,9, ["Bronislav"]),
        Day(4,9, ["Jindřiška", "Henrieta"]),
        Day(5,9, ["Boris"]),
        Day(6,9, ["Boleslav"]),
        Day(7,9, ["Regina", "Regína"]),
        Day(8,9, ["Mariana"]),
        Day(9,9, ["Daniela"]),
        Day(10,9, ["Irma"]),
        Day(11,9, ["Denisa", "Denis"]),
        Day(12,9, ["Marie"]),
        Day(13,9, ["Lubor"]),
        Day(14,9, ["Radka"]),
        Day(15,9, ["Jolana"]),
        Day(16,9, ["Ludmila"]),
        Day(17,9, ["Naděžda"]),
        Day(18,9, ["Kryštof", "Krištof"]),
        Day(19,9, ["Zita"]),
        Day(20,9, ["Oleg"]),
        Day(21,9, ["Matouš"]),
        Day(22,9, ["Darina"]),
        Day(23,9, ["Berta", "Bertina"]),
        Day(24,9, ["Jaromír"]),
        Day(25,9, ["Zlata"]),
        Day(26,9, ["Andrea", "Ondřejka"]),
        Day(27,9, ["Jonáš"]),
        Day(28,9, ["Václav", "Václava"]),
        Day(29,9, ["Michal", "Michael"]),
        Day(30,9, ["Jeroným", "Jeronym", "Jarolím"]),
   

        
        
        Day(1,10, ["Igor"]),
        Day(2,10, ["Olívie", "Olivia", "Oliver"]),
        Day(3,10, ["Bohumil"]),
        Day(4,10, ["František"]),
        Day(5,10, ["Eliška", "Elsa"]),
        Day(6,10, ["Hanuš"]),
        Day(7,10, ["Justýna", "Justina", "Justin"]),
        Day(8,10, ["Věra", "Viera"]),
        Day(9,10, ["Štefan", "Sára"]),
        Day(10,10, ["Marina", "Marína"]),
        Day(11,10, ["Andrej"]),
        Day(12,10, ["Marcel"]),
        Day(13,10, ["Renáta", "Renata"]),
        Day(14,10, ["Agáta"]),
        Day(15,10, ["Tereza"]),
        Day(16,10, ["Havel"]),
        Day(17,10, ["Hedvika"]),
        Day(18,10, ["Lukáš"]),
        Day(19,10, ["Michaela", "Michala"]),
        Day(20,10, ["Vendelín"]),
        Day(21,10, ["Brigita", "Bridget"]),
        Day(22,10, ["Sabina"]),
        Day(23,10, ["Teodor", "Theodor"]),
        Day(24,10, ["Nina"]),
        Day(25,10, ["Beáta", "Beata"]),
        Day(26,10, ["Erik", "Ervín"]),
        Day(27,10, ["Šarlota", "Zoe", "Zoja"]),
        Day(28,10, ["Den vzniku samostatného ČS státu"]),
        Day(29,10, ["Silvie", "Sylva", "Silva"]),
        Day(30,10, ["Tadeáš"]),
        Day(31,10, ["Štěpánka", "Stefanie"]),
    
        
        
        Day(1,11, ["Felix"]),
        Day(2,11, ["--"]),
        Day(3,11, ["Hubert"]),
        Day(4,11, ["Karel"]),
        Day(5,11, ["Miriam"]),
        Day(6,11, ["Liběna"]),
        Day(7,11, ["Saskie"]),
        Day(8,11, ["Bohumír"]),
        Day(9,11, ["Bohdan"]),
        Day(10,11, ["Evžen", "Eugen"]),
        Day(11,11, ["Martin"]),
        Day(12,11, ["Benedikt"]),
        Day(13,11, ["Tibor"]),
        Day(14,11, ["Sáva"]),
        Day(15,11, ["Leopold"]),
        Day(16,11, ["Otmar", "Otomar"]),
        Day(17,11, ["Mahulena"]),
        Day(18,11, ["Romana"]),
        Day(19,11, ["Alžběta", "Elisabeth"]),
        Day(20,11, ["Nikola"]),
        Day(21,11, ["Albert"]),
        Day(22,11, ["Cecílie"]),
        Day(23,11, ["Klement", "Kliment"]),
        Day(24,11, ["Emília", "Emilia"]),
        Day(25,11, ["Kateřina", "Katka", "Katarína"]),
        Day(26,11, ["Artur"]),
        Day(27,11, ["Xenie"]),
        Day(28,11, ["René", "Renée"]),
        Day(29,11, ["Zina"]),
        Day(30,11, ["Ondřej"]),
        
        Day(1,12, ["Iva"]),
        Day(2,12, ["Blanka"]),
        Day(3,12, ["Svatoslav", "Světoslav"]),
        Day(4,12, ["Barbora", "Barbara", "Bára"]),
        Day(5,12, ["Jitka"]),
        Day(6,12, ["Mikuláš", "Mikoláš", "Nikolas"]),
        Day(7,12, ["Ambrož"]),
        Day(8,12, ["Květoslava"]),
        Day(9,12, ["Vratislav"]),
        Day(10,12, ["Julie", "Julia", "Juliana"]),
        Day(11,12, ["Dana"]),
        Day(12,12, ["Simona", "Šimona", "Simeona"]),
        Day(13,12, ["Lucie", "Lucia", "Luciana"]),
        Day(14,12, ["Lýdie", "Lydia", "Lydie"]),
        Day(15,12, ["Radana"]),
        Day(16,12, ["Albína"]),
        Day(17,12, ["Daniel", "Dana"]),
        Day(18,12, ["Miloslav"]),
        Day(19,12, ["Ester"]),
        Day(20,12, ["Dagmar", "Dagmara", "Dáša"]),
        Day(21,12, ["Natálie"]),
        Day(22,12, ["Šimon"]),
        Day(23,12, ["Vlasta"]),
        Day(24,12, ["Adam", "Eva"]),
        Day(25,12, ["1. svátek vánoční"]),
        Day(26,12, ["Štěpán"]),
        Day(27,12, ["Žaneta"]),
        Day(28,12, ["Bohumila"]),
        Day(29,12, ["Judita"]),
        Day(30,12, ["David"]),
        Day(31,12, ["Silvestr", "Silverter", "Sylvestr"])
    ]
    
}

