#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Hold_Browser.H>
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Pixmap.H>

#include <FLU/Flu_DND.h>

static char * garbage_open_xpm[] = {
"30 40 254 2",
"  	c None",
". 	c #E4B4E5",
"+ 	c #DEB0DC",
"@ 	c #E8B9E1",
"# 	c #B3ABE5",
"$ 	c #959DDC",
"% 	c #8A96D7",
"& 	c #7687CA",
"* 	c #97A3E3",
"= 	c #ABA3DD",
"- 	c #A795CD",
"; 	c #9983B8",
"> 	c #9878A6",
", 	c #9C739A",
"' 	c #A57494",
") 	c #C893B4",
"! 	c #DEB9EE",
"~ 	c #A19AD5",
"{ 	c #AFB3EF",
"] 	c #B2B7F3",
"^ 	c #8699DB",
"/ 	c #5C64AA",
"( 	c #6E70B1",
"_ 	c #746FAC",
": 	c #8174AC",
"< 	c #796599",
"[ 	c #856896",
"} 	c #87648C",
"| 	c #93678B",
"1 	c #9E6C8B",
"2 	c #995C74",
"3 	c #B06F84",
"4 	c #C5A3D6",
"5 	c #D4BDF5",
"6 	c #BDB4EE",
"7 	c #8D90CF",
"8 	c #6067AD",
"9 	c #515BA3",
"0 	c #4C5299",
"a 	c #585A9C",
"b 	c #6A65A3",
"c 	c #72669E",
"d 	c #8A73A6",
"e 	c #90709F",
"f 	c #946F99",
"g 	c #986C91",
"h 	c #A56A85",
"i 	c #BF829B",
"j 	c #E9BAEA",
"k 	c #B393C6",
"l 	c #9E8DC5",
"m 	c #615D9B",
"n 	c #727DC0",
"o 	c #8293D4",
"p 	c #7779BA",
"q 	c #8D87C3",
"r 	c #A17CA9",
"s 	c #8B5977",
"t 	c #B07794",
"u 	c #C39ACA",
"v 	c #584A7D",
"w 	c #665B94",
"x 	c #8781BD",
"y 	c #9FA2DF",
"z 	c #7C8ED0",
"A 	c #7E89CB",
"B 	c #9B9EDC",
"C 	c #C3BAF4",
"D 	c #B8A5DC",
"E 	c #8D76AA",
"F 	c #765A86",
"G 	c #5D3E63",
"H 	c #764B6B",
"I 	c #BC8BAF",
"J 	c #B48DBA",
"K 	c #918AC6",
"L 	c #828CCE",
"M 	c #A5AFED",
"N 	c #8B84C1",
"O 	c #60558F",
"P 	c #4C3F72",
"Q 	c #4E3A66",
"R 	c #735279",
"S 	c #D2A4CE",
"T 	c #C999BE",
"U 	c #A1759B",
"V 	c #A480AF",
"W 	c #947CAF",
"X 	c #8C7DB6",
"Y 	c #9B94CF",
"Z 	c #9DB3F2",
"` 	c #3B3879",
" .	c #3A3268",
"..	c #514377",
"+.	c #EEC1E8",
"@.	c #CC98BA",
"#.	c #B383A4",
"$.	c #B386AC",
"%.	c #2E337E",
"&.	c #292A70",
"*.	c #7669A2",
"=.	c #D2AFE4",
"-.	c #EABBDB",
";.	c #CA92AD",
">.	c #CA9BC0",
",.	c #C497C0",
"'.	c #AA8EC2",
").	c #3B428D",
"!.	c #7973B0",
"~.	c #BA9CD0",
"{.	c #D5A8D7",
"].	c #D69DB6",
"^.	c #C58EAB",
"/.	c #C394BC",
"(.	c #B188B4",
"_.	c #8770A5",
":.	c #6E7DC2",
"<.	c #826593",
"[.	c #B77C96",
"}.	c #E4B0CA",
"|.	c #D599AF",
"1.	c #8F6A93",
"2.	c #8688C7",
"3.	c #B4C2FF",
"4.	c #AB718D",
"5.	c #AE677A",
"6.	c #C5869D",
"7.	c #97556B",
"8.	c #A56073",
"9.	c #95607D",
"0.	c #895E82",
"a.	c #8678B1",
"b.	c #898CCB",
"c.	c #8B9BDC",
"d.	c #896B9A",
"e.	c #A67A9F",
"f.	c #B07FA1",
"g.	c #DBA2BA",
"h.	c #AF9CD4",
"i.	c #7781C4",
"j.	c #6371B7",
"k.	c #6062A4",
"l.	c #6762A0",
"m.	c #796CA5",
"n.	c #6B7AC0",
"o.	c #656DB2",
"p.	c #6A6DAE",
"q.	c #8A4A60",
"r.	c #87506C",
"s.	c #6971B6",
"t.	c #8082C2",
"u.	c #9C94D0",
"v.	c #974E5E",
"w.	c #6C3750",
"x.	c #5E3352",
"y.	c #7983C6",
"z.	c #404892",
"A.	c #E9C3F8",
"B.	c #733F59",
"C.	c #7F3E52",
"D.	c #8B3D4A",
"E.	c #603554",
"F.	c #4B2E53",
"G.	c #5F4876",
"H.	c #756295",
"I.	c #8F80B8",
"J.	c #9295D3",
"K.	c #7C7EBE",
"L.	c #653A5A",
"M.	c #6A344D",
"N.	c #763345",
"O.	c #6E3952",
"P.	c #603656",
"Q.	c #4A3763",
"R.	c #7C85C8",
"S.	c #626AAF",
"T.	c #8F92D1",
"U.	c #552B4A",
"V.	c #B17085",
"W.	c #D09DC0",
"X.	c #68334B",
"Y.	c #5B3150",
"Z.	c #523459",
"`.	c #43315D",
" +	c #635286",
".+	c #747EC1",
"++	c #49539D",
"@+	c #6973B7",
"#+	c #CCC4FD",
"$+	c #E0C8FF",
"%+	c #7E6290",
"&+	c #583A5F",
"*+	c #DDACCE",
"=+	c #5A2F4F",
"-+	c #56385E",
";+	c #463460",
">+	c #5F4F83",
",+	c #5C528B",
"'+	c #6B73B7",
")+	c #6978BE",
"!+	c #6F5683",
"~+	c #623758",
"{+	c #6B74B9",
"]+	c #7275B5",
"^+	c #8F88C4",
"/+	c #633857",
"(+	c #534B84",
"_+	c #6E77BB",
":+	c #7577B8",
"<+	c #7E71AA",
"[+	c #5C4E7F",
"}+	c #55375D",
"|+	c #966482",
"1+	c #514881",
"2+	c #6C7BC0",
"3+	c #56416D",
"4+	c #54375B",
"5+	c #5C4672",
"6+	c #534579",
"7+	c #695D96",
"8+	c #68578B",
"9+	c #6B527F",
"0+	c #6B6099",
"a+	c #705D90",
"b+	c #BD87A6",
"c+	c #683D5D",
"d+	c #4F4275",
"e+	c #8285C4",
"f+	c #6977BD",
"g+	c #754F74",
"h+	c #472A4E",
"i+	c #6775BB",
"j+	c #717ABE",
"k+	c #836DA0",
"l+	c #D2A1C6",
"m+	c #372752",
"n+	c #7E79B5",
"o+	c #6870B5",
"p+	c #BE91BA",
"q+	c #6A4467",
"r+	c #807BB8",
"s+	c #684C76",
"t+	c #53355C",
"u+	c #463B6D",
"v+	c #4B427C",
"w+	c #4C4C8D",
"x+	c #494F97",
"y+	c #524E8D",
"z+	c #443C76",
"A+	c #494584",
"B+	c #444589",
"C+	c #471D3B",
"D+	c #534577",
"E+	c #353A83",
"                                          . + @             ",
"                      # $ % & *   = - ; > , , ' )           ",
"                  ! - ~ { ] ^ / ( _ : < [ } | 1 2 3         ",
"                  4 5 6 7 8 9 0 a b c d e f g ' h i         ",
"                j k l m a n o n p q l k r f s s t           ",
"                u v w x y $ z A B C D E F G H I             ",
"            j J [ < ; K 7 L o M { N O P Q R S               ",
"          T U r V W X Y y M Z L a `  ...V j                 ",
"      +.@.#.$.V r E - C ] % 9 %.&.` *.=.                    ",
"    -.;.) >.,.u u '.l q ( 0 ).).a !.- ~.{.+ + +.            ",
"    ].^.@.>./.(.V _.c m a 9 :.* M ~ ; E <.} | ' t [.}.      ",
"    |.[.[.' g 1.<.< *.!.2.* 3.3.] = - ; e } | 1 4.5.6.      ",
"    5.7.8.9.0.} [ E a.N b.% ^ c.7 N X d d.f e.f.[.5.g.      ",
"    g.[.2 9.0., u ~.h.~ 7 i.j./ k.l.m.d > (.e.s 2 8.].      ",
"      -.2 s 0.f k 4 D = B o n.o.p.!.a.W V (., 9.2 8.g.      ",
"      -.q.r.H r J ~.D = B L j.s.t.u.h.4 u J e.s r.v.g.      ",
"        q.w.x.1.S 4 5 C ] y.z.9 2.6 5 A.+ {.$.B.C.D.-.      ",
"        h w.E.F.G.H.I.u.J.n 9 / K.~ h.'.e F L.M.N.v.        ",
"        ^.O.P.F.Q.H.h.# y R.9 S.T.~ h.~.d.G U.w.C.V.        ",
"        W.X.Y.Z.`. +: 6 3..+++@+J.#+$+'.%+&+Y.M.N.6.        ",
"        *+B.=+-+;+>+,+_ T.'+)+.+( Y l >+!+-+~+M.N.@.        ",
"        @ r.=+-+;+v ,+l.b.{+:.A ]+^+:  +!+-+/+M.C.*+        ",
"          9.Y.&+;+P (+l.b._+:.y.:+K <+[+G.}+L.X.7.-.        ",
"          |+~+G ;+P 1+m b.{+2+R.p K m.v 3+4+L.w.7.+.        ",
"          ' L.&+Q.P (+m 2.s.2+R.:+^+m.v 5+}+L.O.2           ",
"          $.E.&+Q.6+7+m 2.{+2+y.:+^+m.8+9+Z./+w.4.          ",
"          /.x.4+;+..0+m 2.@+2+y.]+^+m.a+9+Z.~+w.b+          ",
"          S c+F.;+d+c l.e+s.f+.+]+^+m.H.9+Z.x.O.)           ",
"          + g+h+`...<+b e+s.i+j+]+N <+k+!+Z.U.B.l+          ",
"          +.| F.`...a._ e+'+f+n ]+N a.E !+Z.U.r.+           ",
"            U F.m+d+a.!.t.{+f+.+:+N a.W !+F.U.9.+.          ",
"            e.-+m+..X n+K.o+i+j+p N X ; !+F.U.1             ",
"            p+q+m+6+l ^+:+o+f+j+p r+I.k F h+x.#.            ",
"            + s+;+6+m.n+( 8 j.{+:+r+X W 9+t+c+T             ",
"            + R Q.u+v+w+w+x+++0 w+y+1+..3+&+q+l+            ",
"            + g+Q P 0+= $ 0 ).x+e+~ D _.5+&+q+>.            ",
"            S 4+Q ..,+b p.0 z.9 p x a.H.s+G x.I             ",
"            {.F.m+ .z+A+B+).).z.B+A+v+d+Q.h+C+f.            ",
"              S %+D+v+y+w+%.%.E+B+w+(+..3+R $.              ",
"                                                            "};

static char * garbage_closed_xpm[] = {
"30 40 184 2",
"  	c None",
". 	c #D2D2D2",
"+ 	c #BEBEBE",
"@ 	c #C0C0C0",
"# 	c #C2C2C2",
"$ 	c #C3C3C3",
"% 	c #CBCBCB",
"& 	c #D1D1D1",
"* 	c #EEEEEE",
"= 	c #F4F4F4",
"- 	c #F1F1F1",
"; 	c #F0F0F0",
"> 	c #ECECEC",
", 	c #B6B6B6",
"' 	c #CACACA",
") 	c #E1E1E1",
"! 	c #D6D6D6",
"~ 	c #858585",
"{ 	c #919191",
"] 	c #8A8A8A",
"^ 	c #888888",
"/ 	c #898989",
"( 	c #909090",
"_ 	c #8E8E8E",
": 	c #949494",
"< 	c #B7B7B7",
"[ 	c #C5C5C5",
"} 	c #D3D3D3",
"| 	c #C1C1C1",
"1 	c #B3B3B3",
"2 	c #979797",
"3 	c #9E9E9E",
"4 	c #808080",
"5 	c #797979",
"6 	c #AEAEAE",
"7 	c #AFAFAF",
"8 	c #9F9F9F",
"9 	c #717171",
"0 	c #6E6E6E",
"a 	c #868686",
"b 	c #8C8C8C",
"c 	c #8F8F8F",
"d 	c #989898",
"e 	c #ABABAB",
"f 	c #C8C8C8",
"g 	c #B1B1B1",
"h 	c #AAAAAA",
"i 	c #B2B2B2",
"j 	c #A7A7A7",
"k 	c #969696",
"l 	c #9C9C9C",
"m 	c #A2A2A2",
"n 	c #A8A8A8",
"o 	c #C9C9C9",
"p 	c #ACACAC",
"q 	c #878787",
"r 	c #838383",
"s 	c #7F7F7F",
"t 	c #BABABA",
"u 	c #CDCDCD",
"v 	c #ADADAD",
"w 	c #BDBDBD",
"x 	c #BCBCBC",
"y 	c #B9B9B9",
"z 	c #9A9A9A",
"A 	c #C7C7C7",
"B 	c #DCDCDC",
"C 	c #D8D8D8",
"D 	c #818181",
"E 	c #7D7D7D",
"F 	c #828282",
"G 	c #D4D4D4",
"H 	c #767676",
"I 	c #7A7A7A",
"J 	c #B4B4B4",
"K 	c #EDEDED",
"L 	c #E6E6E6",
"M 	c #DDDDDD",
"N 	c #BBBBBB",
"O 	c #606060",
"P 	c #999999",
"Q 	c #6A6A6A",
"R 	c #6F6F6F",
"S 	c #848484",
"T 	c #A4A4A4",
"U 	c #9D9D9D",
"V 	c #7E7E7E",
"W 	c #757575",
"X 	c #6D6D6D",
"Y 	c #626262",
"Z 	c #5D5D5D",
"` 	c #5B5B5B",
" .	c #575757",
"..	c #E3E3E3",
"+.	c #B0B0B0",
"@.	c #787878",
"#.	c #737373",
"$.	c #747474",
"%.	c #7B7B7B",
"&.	c #777777",
"*.	c #6B6B6B",
"=.	c #5A5A5A",
"-.	c #434343",
";.	c #3B3B3B",
">.	c #3A3A3A",
",.	c #4C4C4C",
"'.	c #545454",
").	c #686868",
"!.	c #727272",
"~.	c #D7D7D7",
"{.	c #939393",
"].	c #929292",
"^.	c #CECECE",
"/.	c #9B9B9B",
"(.	c #A5A5A5",
"_.	c #707070",
":.	c #565656",
"<.	c #666666",
"[.	c #C4C4C4",
"}.	c #CCCCCC",
"|.	c #656565",
"1.	c #E7E7E7",
"2.	c #474747",
"3.	c #444444",
"4.	c #959595",
"5.	c #E9E9E9",
"6.	c #EAEAEA",
"7.	c #F2F2F2",
"8.	c #454545",
"9.	c #484848",
"0.	c #646464",
"a.	c #424242",
"b.	c #3F3F3F",
"c.	c #A9A9A9",
"d.	c #404040",
"e.	c #3E3E3E",
"f.	c #464646",
"g.	c #363636",
"h.	c #4B4B4B",
"i.	c #8B8B8B",
"j.	c #D9D9D9",
"k.	c #C6C6C6",
"l.	c #A3A3A3",
"m.	c #F3F3F3",
"n.	c #F8F8F8",
"o.	c #FFFFFF",
"p.	c #A6A6A6",
"q.	c #3C3C3C",
"r.	c #3D3D3D",
"s.	c #676767",
"t.	c #505050",
"u.	c #636363",
"v.	c #5F5F5F",
"w.	c #414141",
"x.	c #494949",
"y.	c #515151",
"z.	c #4F4F4F",
"A.	c #4A4A4A",
"B.	c #555555",
"C.	c #5E5E5E",
"D.	c #525252",
"E.	c #616161",
"F.	c #393939",
"G.	c #373737",
"H.	c #343434",
"I.	c #DADADA",
"J.	c #696969",
"K.	c #353535",
"L.	c #A1A1A1",
"M.	c #383838",
"N.	c #A0A0A0",
"O.	c #313131",
"P.	c #B5B5B5",
"Q.	c #303030",
"R.	c #535353",
"S.	c #585858",
"T.	c #6C6C6C",
"U.	c #5C5C5C",
"V.	c #BFBFBF",
"W.	c #595959",
"X.	c #282828",
"Y.	c #323232",
"Z.	c #292929",
"`.	c #4E4E4E",
"                                                            ",
"                                                            ",
"                                                            ",
"                        . + @ # $ %                         ",
"                      & * = - ; > , '                       ",
"                ) ! @ ~ { ] ^ / ( _ : < [ & )               ",
"        } | 1 2 2 3 4 5 / 3 6 7 8 9 0 a b c / d e f         ",
"      g h i g j 3 k l m n + % o # p k ^ q r s s 4 ~ t       ",
"    u d v t w x y 6 z g # # $ # | A B C < j c D D E F G     ",
"    H I ( 8 n h t w J G > ; ; ; K L M . N 3 c ] 5 _ O a     ",
"    P Q R H E S ] ] _ P T U _ V W X Y Z ` Z  .` Q D { ..    ",
"      +.@.#.$.E 4 s %.&.9 *.=.-.;.>.-.,.'.` ).!.S %.~.      ",
"      t W H @.{.1 1 g 6 n 3 ].~ D 4 q _ k h { !.!.#..       ",
"      N 0 *.&.c + ^.! ~.^.y 8 { ].2 /.8 (.T a !.*._.!       ",
"      | :.Z <.8 1 [.} & }.7 ].k p @ A % w h { Y |.Z ~.      ",
"      1.2.3.3.4.u C = - 5./.Z 5 @ 6.7.= ~.} : 8.9.:.        ",
"        0.a.b.b.O F c.@ @ l %.S +.A [.6 / ).-.d.e.&.        ",
"        ] f.b.g.h.i.. j.. T H k # k.A x %.9.g.3.f.m         ",
"        l.;.e.e.-.R U m.n.{.$./.u o.n.p._.-.b.q.e.t         ",
"        t ;.r.8.-.Q <.z y ].T m k [ l ).s.a.f.;.8.%         ",
"        u t.r.8.-.u.)._ J 2 v 7 P J r Q 0.a.8.;.Z M         ",
"        M u.q.f.a.:.v.] J z c.e l J 4 Y :.a.8.e.R           ",
"        5.).w.x.-.y.` a 1 2 n p l i 5 Z z.-.A.3.H           ",
"          $.3.x.a.B.Z ~ 6 {.c.p /.6 H C.D.3.9.f.b           ",
"          q -.f.a.E.#.F 7 d n c.l +.@.#.` -.8.f.n           ",
"          P r.3.a.Y &.r g : j c.2 i I 5 Z a.a.h.1           ",
"          7 F.e.d.E.s a 6 ].m m d g s 4 ` d.F.t.$           ",
"          [ A.G.d.Y _ i.e {.3 8 P 6 ~ c Z d.H.C.I.          ",
"          I.J.K.e.E.P k c.k 8 L.P v _ z ` r.K.$.            ",
"          ..4 K.M.E.U d c.4.8 L.3 e : U ` F.g.]             ",
"          6.a >.K.|.L.l T { U 8 8 h z N.Z g.q.P             ",
"            z h.O.).w c.P { N.N.8 8 n P.v.Q.A.g             ",
"            @ R.-.O _ l ( ^ k d /.l T k C.b. .&             ",
"            k.` 3.,.S.v.Y ).T.*.|.E.U.B.,.2.E.~.            ",
"            V.B.x.'.q j.w E.W.!.1 % o 4 y.9.C.&             ",
"            6 q.h.S.X q ] ).u.E l.p.4.$.=.A.3.|             ",
"            J X.Q.-.z.W.`  .` U.=.W. .y.-.Y.Z.,             ",
"              x $.B.'.).C.a.8.`.=.Y v.'.R.J.i               ",
"                                                            ",
"                                                            "};

Fl_Pixmap garbageOpen( (char*const*)garbage_open_xpm );
Fl_Pixmap garbageClosed( (char*const*)garbage_closed_xpm );

class DND_Browser : public Fl_Hold_Browser, public Flu_DND
{
public:

  DND_Browser( int x, int y, int w, int h, const char *l = 0 )
    : Fl_Hold_Browser( x, y, w, h, l ), Flu_DND("DND_Browser")
  {
    dnd_allow_type( "DND_Browser" ); // allow items from the browser to be moved within the browser
    dnd_allow_type( "DND_Button" ); // allow DND_Button items to be dropped on the browser
    dnd_allow_text( true ); // allow normal FLTK DND text to be dropped on the browser
  }

  int handle( int event )
  {
    // check for dnd handling first
    if( dnd_handle( event ) )
      return 1;

    // if the mouse starts dragging and we are not already dragging something,
    // start DND
    if( event == FL_DRAG && !dnd_is_dragging() )
      {
	dnd_grab( this, "DND_Browser" );
	return 1;
      }
    else
      return Fl_Hold_Browser::handle( event );
  }

protected:

  void on_dnd_drop( const Flu_DND_Event *e )
  {
    // if the dnd event is not FLTK text
    if( e->event_is_other() )
      {
	// if the object being dragged is us (the browser), then just moved the
	// selected entry to the bottom of the browser
	if( e->data() == this )
	  {
	    int selected = value();
	    char *t = strdup( text(selected) );
	    remove( selected );
	    add( t );
	    free( t );
	  }
	// otherwise if it is another browser, copy its selected entry to the
	// bottom of us
	else if( e->is_data_type( "DND_Browser" ) )
	  {
	    Fl_Hold_Browser *b = (Fl_Hold_Browser*)e->data();
	    add( b->text( b->value() ) );
	  }
	// otherwise if it is a button, then copy the button's label to
	// the bottom of us
	else if( e->is_data_type( "DND_Button" ) )
	  {
	    Fl_Button *b = (Fl_Button*)e->data();
	    add( b->label() );
	  }
      }
    else
      {
	// the event is an FLTK text event. just add the text to the bottom of the browser
	add( e->text() );
      }
  }

};

// this class is used both to drag a new item into a browser, and also to 
// drop an item from a browser into the trash. it is dual-purpose depending
// on whether dnd_allow_dragging() is true or false.
class DND_Button : public Fl_Button, public Flu_DND
{
public:

  DND_Button( int x, int y, int w, int h, const char *l = 0 )
    : Fl_Button( x, y, w, h, l ), Flu_DND("DND_Button")
  {
    dndimages( 0, 0 );
    dnd_allow_text( false ); // don't allow anything to be dropped on us
  }

  int handle( int event )
  {
    // check for dnd handling first
    if( dnd_handle( event ) )
      return 1;

    // only start dragging if we are allowed to
    if( event == FL_PUSH && Flu_DND::dnd_allow_dragging() )
      {
	dnd_grab( this, "DND_Button" );
	return 1;
      }
    else
      return Fl_Button::handle( event );
  }

  // the garbage button has 2 images depending on whether something
  // is being dragged over it or not
  inline void dndimages( Fl_Image *i1, Fl_Image *i2 )
  { 
    dndImage[0] = i1; dndImage[1] = i2;
    if( dndImage[1] )
      image( dndImage[1] );
  }

  inline void dnd_allow_dropping( bool b )
  { Flu_DND::dnd_allow_dropping( b ); }

  inline void dnd_allow_dragging( bool b )
  { Flu_DND::dnd_allow_dragging( b ); }

  inline void dnd_allow_type( const char *t )
  { Flu_DND::dnd_allow_type( t ); }

protected:

  Fl_Image *dndImage[2];

  void on_dnd_enter()
  {
    // change the image when something is dragged in
    image( dndImage[0] );
    redraw();
  }

  void on_dnd_leave()
  {
    // change the image when something is dragged out
    image( dndImage[1] );
    redraw();
  }

  void on_dnd_release()
  {
    // change the image when something is dropped
    image( dndImage[1] );
    redraw();
  }

};

// this is called when something is dropped on the grabage
// remove the selected entry from the browser
static void dndTrashButtonCB( const Flu_DND_Event *e, void *arg )
{
  if( !e->is_data_type( "DND_Browser" ) )
    return;
  DND_Browser *b = (DND_Browser*)e->data();
  if( b->value() )
    b->remove( b->value() );
}

// this is called when the garbage button is pushed
// remove the selected entry from the browser
static void trashButtonCB( Fl_Widget *w, void *arg )
{
  DND_Browser *b = (DND_Browser*)arg;
  if( b->value() )
    b->remove( b->value() );
}

int main( int argc, char **argv )
{
  Fl_Single_Window *win = new Fl_Single_Window( 100, 100, 300, 300, "Flu_DND_Try" );

  // the trash can automatically allows dropping. disallow dragging
  DND_Button *trash = new DND_Button( 10, 5, 30, 40 );
  trash->dnd_allow_dragging( false );

  // the "new" button automatically allows dragging. disallow dropping
  // disable the dropping of things on the "new" button
  DND_Button *newItem = new DND_Button( 80, 10, 60, 30, "New" );
  newItem->dnd_allow_dropping( false );

  DND_Browser *s = new DND_Browser( 10, 50, 280, 240 );
  s->add( "one" );
  s->add( "two" );
  s->add( "three" );

  //b->box( FL_FLAT_BOX );
  //b->down_box( FL_FLAT_BOX );
  trash->dndimages( &garbageOpen, &garbageClosed );
  trash->dnd_allow_type( "DND_Browser" );
  trash->dnd_callback( dndTrashButtonCB );
  trash->callback( trashButtonCB, s );

  Fl_Input *in = new Fl_Input( 150, 10, 100, 30 );

  win->end();

  win->show();

  // allow fltk text dnd
  Fl::dnd_text_ops( 1 );

  return Fl::run();
}
