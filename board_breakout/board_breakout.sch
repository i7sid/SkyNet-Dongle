<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="6.5.0">
<drawing>
<settings>
<setting alwaysvectorfont="no"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="frames">
<description>&lt;b&gt;Frames for Sheet and Layout&lt;/b&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="A4L-LOC">
<wire x1="256.54" y1="3.81" x2="256.54" y2="8.89" width="0.1016" layer="94"/>
<wire x1="256.54" y1="8.89" x2="256.54" y2="13.97" width="0.1016" layer="94"/>
<wire x1="256.54" y1="13.97" x2="256.54" y2="19.05" width="0.1016" layer="94"/>
<wire x1="256.54" y1="19.05" x2="256.54" y2="24.13" width="0.1016" layer="94"/>
<wire x1="161.29" y1="3.81" x2="161.29" y2="24.13" width="0.1016" layer="94"/>
<wire x1="161.29" y1="24.13" x2="215.265" y2="24.13" width="0.1016" layer="94"/>
<wire x1="215.265" y1="24.13" x2="256.54" y2="24.13" width="0.1016" layer="94"/>
<wire x1="246.38" y1="3.81" x2="246.38" y2="8.89" width="0.1016" layer="94"/>
<wire x1="246.38" y1="8.89" x2="256.54" y2="8.89" width="0.1016" layer="94"/>
<wire x1="246.38" y1="8.89" x2="215.265" y2="8.89" width="0.1016" layer="94"/>
<wire x1="215.265" y1="8.89" x2="215.265" y2="3.81" width="0.1016" layer="94"/>
<wire x1="215.265" y1="8.89" x2="215.265" y2="13.97" width="0.1016" layer="94"/>
<wire x1="215.265" y1="13.97" x2="256.54" y2="13.97" width="0.1016" layer="94"/>
<wire x1="215.265" y1="13.97" x2="215.265" y2="19.05" width="0.1016" layer="94"/>
<wire x1="215.265" y1="19.05" x2="256.54" y2="19.05" width="0.1016" layer="94"/>
<wire x1="215.265" y1="19.05" x2="215.265" y2="24.13" width="0.1016" layer="94"/>
<text x="217.17" y="15.24" size="2.54" layer="94" font="vector">&gt;DRAWING_NAME</text>
<text x="217.17" y="10.16" size="2.286" layer="94" font="vector">&gt;LAST_DATE_TIME</text>
<text x="230.505" y="5.08" size="2.54" layer="94" font="vector">&gt;SHEET</text>
<text x="216.916" y="4.953" size="2.54" layer="94" font="vector">Sheet:</text>
<frame x1="0" y1="0" x2="260.35" y2="179.07" columns="6" rows="4" layer="94"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="A4L-LOC" prefix="FRAME" uservalue="yes">
<description>&lt;b&gt;FRAME&lt;/b&gt;&lt;p&gt;
DIN A4, landscape with location and doc. field</description>
<gates>
<gate name="G$1" symbol="A4L-LOC" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="pinhead">
<description>&lt;b&gt;Simple Pin Header Connectors&lt;/b&gt;&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="2X06">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt; - 0.1"</description>
<wire x1="-7.62" y1="-2.54" x2="-7.62" y2="2.54" width="0.254" layer="21"/>
<wire x1="-7.62" y1="-2.54" x2="7.62" y2="-2.54" width="0.254" layer="21"/>
<wire x1="-7.62" y1="2.54" x2="7.62" y2="2.54" width="0.254" layer="21"/>
<wire x1="7.62" y1="2.54" x2="7.62" y2="-2.54" width="0.254" layer="21"/>
<pad name="1" x="-6.35" y="-1.27" drill="1.016" shape="square"/>
<pad name="2" x="-6.35" y="1.27" drill="1.016" shape="octagon"/>
<pad name="3" x="-3.81" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="4" x="-3.81" y="1.27" drill="1.016" shape="octagon"/>
<pad name="5" x="-1.27" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="6" x="-1.27" y="1.27" drill="1.016" shape="octagon"/>
<pad name="7" x="1.27" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="8" x="1.27" y="1.27" drill="1.016" shape="octagon"/>
<pad name="9" x="3.81" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="10" x="3.81" y="1.27" drill="1.016" shape="octagon"/>
<pad name="11" x="6.35" y="-1.27" drill="1.016" shape="octagon"/>
<pad name="12" x="6.35" y="1.27" drill="1.016" shape="octagon"/>
<text x="-7.62" y="3.175" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-7.62" y="-4.445" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="-6.604" y1="-1.524" x2="-6.096" y2="-1.016" layer="51"/>
<rectangle x1="-6.604" y1="1.016" x2="-6.096" y2="1.524" layer="51"/>
<rectangle x1="-4.064" y1="1.016" x2="-3.556" y2="1.524" layer="51"/>
<rectangle x1="-4.064" y1="-1.524" x2="-3.556" y2="-1.016" layer="51"/>
<rectangle x1="-1.524" y1="1.016" x2="-1.016" y2="1.524" layer="51"/>
<rectangle x1="-1.524" y1="-1.524" x2="-1.016" y2="-1.016" layer="51"/>
<rectangle x1="1.016" y1="1.016" x2="1.524" y2="1.524" layer="51"/>
<rectangle x1="3.556" y1="1.016" x2="4.064" y2="1.524" layer="51"/>
<rectangle x1="1.016" y1="-1.524" x2="1.524" y2="-1.016" layer="51"/>
<rectangle x1="3.556" y1="-1.524" x2="4.064" y2="-1.016" layer="51"/>
<rectangle x1="6.096" y1="1.016" x2="6.604" y2="1.524" layer="51"/>
<rectangle x1="6.096" y1="-1.524" x2="6.604" y2="-1.016" layer="51"/>
</package>
<package name="2X06M">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt; - 2MM"</description>
<wire x1="6.5" y1="2.25" x2="6.5" y2="-2.25" width="0.2032" layer="21"/>
<wire x1="6.5" y1="-2.25" x2="-6.25" y2="-2.25" width="0.2032" layer="21"/>
<wire x1="-6.25" y1="-2.25" x2="-6.25" y2="2.25" width="0.2032" layer="21"/>
<wire x1="-6.25" y1="2.25" x2="6.5" y2="2.25" width="0.2032" layer="21"/>
<pad name="1" x="-5" y="-1" drill="0.9144" shape="square"/>
<pad name="2" x="-5" y="1" drill="0.9144"/>
<pad name="3" x="-3" y="-1" drill="0.9144"/>
<pad name="4" x="-3" y="1" drill="0.9144"/>
<pad name="5" x="-1" y="-1" drill="0.9144"/>
<pad name="6" x="-1" y="1" drill="0.9144"/>
<pad name="7" x="1" y="-1" drill="0.9144"/>
<pad name="8" x="1" y="1" drill="0.9144"/>
<pad name="9" x="3" y="-1" drill="0.9144"/>
<pad name="10" x="3" y="1" drill="0.9144"/>
<pad name="11" x="5" y="-1" drill="0.9144"/>
<pad name="12" x="5" y="1" drill="0.9144"/>
<text x="-7" y="-2" size="1.016" layer="25" ratio="14" rot="R90">&gt;NAME</text>
<text x="8" y="-2" size="0.8128" layer="27" ratio="10" rot="R90">&gt;VALUE</text>
<rectangle x1="-5.25" y1="0.75" x2="-4.75" y2="1.25" layer="51"/>
<rectangle x1="-5.25" y1="-1.25" x2="-4.75" y2="-0.75" layer="51"/>
<rectangle x1="-3.25" y1="0.75" x2="-2.75" y2="1.25" layer="51"/>
<rectangle x1="-3.25" y1="-1.25" x2="-2.75" y2="-0.75" layer="51"/>
<rectangle x1="-1.25" y1="0.75" x2="-0.75" y2="1.25" layer="51"/>
<rectangle x1="-1.25" y1="-1.25" x2="-0.75" y2="-0.75" layer="51"/>
<rectangle x1="0.75" y1="0.75" x2="1.25" y2="1.25" layer="51"/>
<rectangle x1="0.75" y1="-1.25" x2="1.25" y2="-0.75" layer="51"/>
<rectangle x1="2.75" y1="0.75" x2="3.25" y2="1.25" layer="51"/>
<rectangle x1="2.75" y1="-1.25" x2="3.25" y2="-0.75" layer="51"/>
<rectangle x1="4.75" y1="0.75" x2="5.25" y2="1.25" layer="51"/>
<rectangle x1="4.75" y1="-1.25" x2="5.25" y2="-0.75" layer="51"/>
</package>
<package name="1X08">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt; - 0.1"</description>
<wire x1="-10.16" y1="1.27" x2="-10.16" y2="-1.27" width="0.254" layer="21"/>
<wire x1="10.16" y1="1.27" x2="10.16" y2="-1.27" width="0.254" layer="21"/>
<wire x1="-10.16" y1="1.27" x2="-7.62" y2="1.27" width="0.254" layer="21"/>
<wire x1="-7.62" y1="1.27" x2="10.16" y2="1.27" width="0.254" layer="21"/>
<wire x1="10.16" y1="-1.27" x2="-10.16" y2="-1.27" width="0.254" layer="21"/>
<wire x1="-7.62" y1="1.27" x2="-7.62" y2="-1.27" width="0.254" layer="21"/>
<pad name="1" x="-8.89" y="0" drill="1.016" shape="square" rot="R90"/>
<pad name="2" x="-6.35" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="3" x="-3.81" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="4" x="-1.27" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="5" x="1.27" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="6" x="3.81" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="7" x="6.35" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="8" x="8.89" y="0" drill="1.016" shape="octagon" rot="R90"/>
<text x="-10.2362" y="1.8288" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-10.16" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="6.096" y1="-0.254" x2="6.604" y2="0.254" layer="51"/>
<rectangle x1="3.556" y1="-0.254" x2="4.064" y2="0.254" layer="51"/>
<rectangle x1="1.016" y1="-0.254" x2="1.524" y2="0.254" layer="51"/>
<rectangle x1="-1.524" y1="-0.254" x2="-1.016" y2="0.254" layer="51"/>
<rectangle x1="-4.064" y1="-0.254" x2="-3.556" y2="0.254" layer="51"/>
<rectangle x1="-6.604" y1="-0.254" x2="-6.096" y2="0.254" layer="51"/>
<rectangle x1="-9.144" y1="-0.254" x2="-8.636" y2="0.254" layer="51"/>
<rectangle x1="8.636" y1="-0.254" x2="9.144" y2="0.254" layer="51"/>
</package>
<package name="1X08M">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt; - 2MM"</description>
<wire x1="8.25" y1="1.25" x2="8.25" y2="-1.25" width="0.2032" layer="21"/>
<wire x1="8.25" y1="-1.25" x2="-8.25" y2="-1.25" width="0.2032" layer="21"/>
<wire x1="-8.25" y1="-1.25" x2="-8.25" y2="1.25" width="0.2032" layer="21"/>
<wire x1="-8.25" y1="1.25" x2="8.25" y2="1.25" width="0.2032" layer="21"/>
<pad name="1" x="-7" y="0" drill="0.9144" shape="square"/>
<pad name="2" x="-5" y="0" drill="0.9144"/>
<pad name="3" x="-3" y="0" drill="0.9144" rot="R270"/>
<pad name="4" x="-1" y="0" drill="0.9144" rot="R270"/>
<pad name="5" x="1" y="0" drill="0.9144" rot="R270"/>
<pad name="6" x="3" y="0" drill="0.9144" rot="R270"/>
<pad name="7" x="5" y="0" drill="0.9144" rot="R270"/>
<pad name="8" x="7" y="0" drill="0.9144" rot="R270"/>
<text x="-9" y="-1" size="1.016" layer="25" ratio="14" rot="R90">&gt;NAME</text>
<text x="10" y="-1" size="0.8128" layer="27" ratio="10" rot="R90">&gt;VALUE</text>
<rectangle x1="-5.25" y1="-0.25" x2="-4.75" y2="0.25" layer="51"/>
<rectangle x1="-7.25" y1="-0.25" x2="-6.75" y2="0.25" layer="51"/>
<rectangle x1="-1.25" y1="-0.25" x2="-0.75" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="-3.25" y1="-0.25" x2="-2.75" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="2.75" y1="-0.25" x2="3.25" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="0.75" y1="-0.25" x2="1.25" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="6.75" y1="-0.25" x2="7.25" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="4.75" y1="-0.25" x2="5.25" y2="0.25" layer="51" rot="R270"/>
</package>
<package name="1X10">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt; - 0.1"</description>
<wire x1="12.7" y1="-1.27" x2="-12.7" y2="-1.27" width="0.254" layer="21"/>
<wire x1="-12.7" y1="1.27" x2="-12.7" y2="-1.27" width="0.254" layer="21"/>
<wire x1="12.7" y1="1.27" x2="12.7" y2="-1.27" width="0.254" layer="21"/>
<wire x1="-12.7" y1="1.27" x2="-10.16" y2="1.27" width="0.254" layer="21"/>
<wire x1="-10.16" y1="1.27" x2="12.7" y2="1.27" width="0.254" layer="21"/>
<wire x1="-10.16" y1="1.27" x2="-10.16" y2="-1.27" width="0.254" layer="21"/>
<pad name="1" x="-11.43" y="0" drill="1.016" shape="square" rot="R90"/>
<pad name="2" x="-8.89" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="3" x="-6.35" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="4" x="-3.81" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="5" x="-1.27" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="6" x="1.27" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="7" x="3.81" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="8" x="6.35" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="9" x="8.89" y="0" drill="1.016" shape="octagon" rot="R90"/>
<pad name="10" x="11.43" y="0" drill="1.016" shape="octagon" rot="R90"/>
<text x="-12.7762" y="1.8288" size="1.27" layer="25" ratio="10">&gt;NAME</text>
<text x="-12.7" y="-3.175" size="1.27" layer="27">&gt;VALUE</text>
<rectangle x1="8.636" y1="-0.254" x2="9.144" y2="0.254" layer="51"/>
<rectangle x1="6.096" y1="-0.254" x2="6.604" y2="0.254" layer="51"/>
<rectangle x1="3.556" y1="-0.254" x2="4.064" y2="0.254" layer="51"/>
<rectangle x1="1.016" y1="-0.254" x2="1.524" y2="0.254" layer="51"/>
<rectangle x1="-1.524" y1="-0.254" x2="-1.016" y2="0.254" layer="51"/>
<rectangle x1="-4.064" y1="-0.254" x2="-3.556" y2="0.254" layer="51"/>
<rectangle x1="-6.604" y1="-0.254" x2="-6.096" y2="0.254" layer="51"/>
<rectangle x1="-9.144" y1="-0.254" x2="-8.636" y2="0.254" layer="51"/>
<rectangle x1="-11.684" y1="-0.254" x2="-11.176" y2="0.254" layer="51"/>
<rectangle x1="11.176" y1="-0.254" x2="11.684" y2="0.254" layer="51"/>
</package>
<package name="1X10M">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt; - 2MM"</description>
<wire x1="10.25" y1="1.25" x2="10.25" y2="-1.25" width="0.2032" layer="21"/>
<wire x1="10.25" y1="-1.25" x2="-10.25" y2="-1.25" width="0.2032" layer="21"/>
<wire x1="-10.25" y1="-1.25" x2="-10.25" y2="1.25" width="0.2032" layer="21"/>
<wire x1="-10.25" y1="1.25" x2="10.25" y2="1.25" width="0.2032" layer="21"/>
<pad name="1" x="-9" y="0" drill="0.9144" shape="square"/>
<pad name="2" x="-7" y="0" drill="0.9144"/>
<pad name="3" x="-5" y="0" drill="0.9144" rot="R270"/>
<pad name="4" x="-3" y="0" drill="0.9144" rot="R270"/>
<pad name="5" x="-1" y="0" drill="0.9144" rot="R270"/>
<pad name="6" x="1" y="0" drill="0.9144" rot="R270"/>
<pad name="7" x="3" y="0" drill="0.9144" rot="R270"/>
<pad name="8" x="5" y="0" drill="0.9144" rot="R270"/>
<pad name="9" x="7" y="0" drill="0.9144" rot="R270"/>
<pad name="10" x="9" y="0" drill="0.9144" rot="R270"/>
<text x="-11" y="-1" size="1.016" layer="25" ratio="14" rot="R90">&gt;NAME</text>
<text x="12" y="-1" size="0.8128" layer="27" ratio="10" rot="R90">&gt;VALUE</text>
<rectangle x1="-7.25" y1="-0.25" x2="-6.75" y2="0.25" layer="51"/>
<rectangle x1="-9.25" y1="-0.25" x2="-8.75" y2="0.25" layer="51"/>
<rectangle x1="-3.25" y1="-0.25" x2="-2.75" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="-5.25" y1="-0.25" x2="-4.75" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="0.75" y1="-0.25" x2="1.25" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="-1.25" y1="-0.25" x2="-0.75" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="4.75" y1="-0.25" x2="5.25" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="2.75" y1="-0.25" x2="3.25" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="8.75" y1="-0.25" x2="9.25" y2="0.25" layer="51" rot="R270"/>
<rectangle x1="6.75" y1="-0.25" x2="7.25" y2="0.25" layer="51" rot="R270"/>
</package>
</packages>
<symbols>
<symbol name="PINH2X6">
<wire x1="-6.35" y1="-10.16" x2="8.89" y2="-10.16" width="0.4064" layer="94"/>
<wire x1="8.89" y1="-10.16" x2="8.89" y2="7.62" width="0.4064" layer="94"/>
<wire x1="8.89" y1="7.62" x2="-6.35" y2="7.62" width="0.4064" layer="94"/>
<wire x1="-6.35" y1="7.62" x2="-6.35" y2="-10.16" width="0.4064" layer="94"/>
<text x="-6.35" y="8.255" size="1.778" layer="95">&gt;NAME</text>
<text x="-6.35" y="-12.7" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-2.54" y="5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="2" x="5.08" y="5.08" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="3" x="-2.54" y="2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="4" x="5.08" y="2.54" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="5" x="-2.54" y="0" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="6" x="5.08" y="0" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="7" x="-2.54" y="-2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="8" x="5.08" y="-2.54" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="9" x="-2.54" y="-5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="10" x="5.08" y="-5.08" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
<pin name="11" x="-2.54" y="-7.62" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="12" x="5.08" y="-7.62" visible="pad" length="short" direction="pas" function="dot" rot="R180"/>
</symbol>
<symbol name="PINHD8">
<wire x1="-6.35" y1="-10.16" x2="1.27" y2="-10.16" width="0.4064" layer="94"/>
<wire x1="1.27" y1="-10.16" x2="1.27" y2="12.7" width="0.4064" layer="94"/>
<wire x1="1.27" y1="12.7" x2="-6.35" y2="12.7" width="0.4064" layer="94"/>
<wire x1="-6.35" y1="12.7" x2="-6.35" y2="-10.16" width="0.4064" layer="94"/>
<text x="-6.35" y="13.335" size="1.778" layer="95">&gt;NAME</text>
<text x="-6.35" y="-12.7" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-2.54" y="10.16" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="2" x="-2.54" y="7.62" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="3" x="-2.54" y="5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="4" x="-2.54" y="2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="5" x="-2.54" y="0" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="6" x="-2.54" y="-2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="7" x="-2.54" y="-5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="8" x="-2.54" y="-7.62" visible="pad" length="short" direction="pas" function="dot"/>
</symbol>
<symbol name="PINHD10">
<wire x1="-6.35" y1="-15.24" x2="1.27" y2="-15.24" width="0.4064" layer="94"/>
<wire x1="1.27" y1="-15.24" x2="1.27" y2="12.7" width="0.4064" layer="94"/>
<wire x1="1.27" y1="12.7" x2="-6.35" y2="12.7" width="0.4064" layer="94"/>
<wire x1="-6.35" y1="12.7" x2="-6.35" y2="-15.24" width="0.4064" layer="94"/>
<text x="-6.35" y="13.335" size="1.778" layer="95">&gt;NAME</text>
<text x="-6.35" y="-17.78" size="1.778" layer="96">&gt;VALUE</text>
<pin name="1" x="-2.54" y="10.16" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="2" x="-2.54" y="7.62" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="3" x="-2.54" y="5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="4" x="-2.54" y="2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="5" x="-2.54" y="0" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="6" x="-2.54" y="-2.54" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="7" x="-2.54" y="-5.08" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="8" x="-2.54" y="-7.62" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="9" x="-2.54" y="-10.16" visible="pad" length="short" direction="pas" function="dot"/>
<pin name="10" x="-2.54" y="-12.7" visible="pad" length="short" direction="pas" function="dot"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="PINHD-2X6" prefix="JP" uservalue="yes">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="A" symbol="PINH2X6" x="0" y="0"/>
</gates>
<devices>
<device name="" package="2X06">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="10" pad="10"/>
<connect gate="A" pin="11" pad="11"/>
<connect gate="A" pin="12" pad="12"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
<connect gate="A" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="M" package="2X06M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="10" pad="10"/>
<connect gate="A" pin="11" pad="11"/>
<connect gate="A" pin="12" pad="12"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
<connect gate="A" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="PINHD-1X8" prefix="JP" uservalue="yes">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="A" symbol="PINHD8" x="0" y="0"/>
</gates>
<devices>
<device name="" package="1X08">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="M" package="1X08M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="PINHD-1X10" prefix="JP" uservalue="yes">
<description>&lt;b&gt;PIN HEADER&lt;/b&gt;</description>
<gates>
<gate name="A" symbol="PINHD10" x="0" y="0"/>
</gates>
<devices>
<device name="" package="1X10">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="10" pad="10"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
<connect gate="A" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
<device name="M" package="1X10M">
<connects>
<connect gate="A" pin="1" pad="1"/>
<connect gate="A" pin="10" pad="10"/>
<connect gate="A" pin="2" pad="2"/>
<connect gate="A" pin="3" pad="3"/>
<connect gate="A" pin="4" pad="4"/>
<connect gate="A" pin="5" pad="5"/>
<connect gate="A" pin="6" pad="6"/>
<connect gate="A" pin="7" pad="7"/>
<connect gate="A" pin="8" pad="8"/>
<connect gate="A" pin="9" pad="9"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="supply1">
<description>&lt;b&gt;Supply Symbols&lt;/b&gt;&lt;p&gt;
 GND, VCC, 0V, +5V, -5V, etc.&lt;p&gt;
 Please keep in mind, that these devices are necessary for the
 automatic wiring of the supply signals.&lt;p&gt;
 The pin name defined in the symbol is identical to the net which is to be wired automatically.&lt;p&gt;
 In this library the device names are the same as the pin names of the symbols, therefore the correct signal names appear next to the supply symbols in the schematic.&lt;p&gt;
 &lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
</packages>
<symbols>
<symbol name="VCC">
<wire x1="1.27" y1="-1.905" x2="0" y2="0" width="0.254" layer="94"/>
<wire x1="0" y1="0" x2="-1.27" y2="-1.905" width="0.254" layer="94"/>
<text x="-2.54" y="-2.54" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="VCC" x="0" y="-2.54" visible="off" length="short" direction="sup" rot="R90"/>
</symbol>
<symbol name="GND">
<wire x1="-1.905" y1="0" x2="1.905" y2="0" width="0.254" layer="94"/>
<text x="-2.54" y="-2.54" size="1.778" layer="96">&gt;VALUE</text>
<pin name="GND" x="0" y="2.54" visible="off" length="short" direction="sup" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="VCC" prefix="P+">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="VCC" symbol="VCC" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
<deviceset name="GND" prefix="GND">
<description>&lt;b&gt;SUPPLY SYMBOL&lt;/b&gt;</description>
<gates>
<gate name="1" symbol="GND" x="0" y="0"/>
</gates>
<devices>
<device name="">
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="FRAME1" library="frames" deviceset="A4L-LOC" device=""/>
<part name="JP1" library="pinhead" deviceset="PINHD-2X6" device="M" value="SKYNET"/>
<part name="P+1" library="supply1" deviceset="VCC" device=""/>
<part name="GND1" library="supply1" deviceset="GND" device=""/>
<part name="JP2" library="pinhead" deviceset="PINHD-1X8" device="" value="JTAG"/>
<part name="JP3" library="pinhead" deviceset="PINHD-1X10" device=""/>
</parts>
<sheets>
<sheet>
<plain>
<text x="7.62" y="91.44" size="2.54" layer="91">Input</text>
<text x="132.08" y="91.44" size="2.54" layer="91">JTAG</text>
<text x="132.08" y="83.82" size="2.54" layer="91">Periph</text>
</plain>
<instances>
<instance part="FRAME1" gate="G$1" x="0" y="0"/>
<instance part="JP1" gate="A" x="60.96" y="134.62"/>
<instance part="P+1" gate="VCC" x="38.1" y="147.32"/>
<instance part="GND1" gate="1" x="76.2" y="121.92"/>
<instance part="JP2" gate="A" x="203.2" y="132.08"/>
<instance part="JP3" gate="A" x="205.74" y="55.88"/>
</instances>
<busses>
</busses>
<nets>
<net name="3V3" class="0">
<segment>
<pinref part="P+1" gate="VCC" pin="VCC"/>
<pinref part="JP1" gate="A" pin="11"/>
<wire x1="38.1" y1="144.78" x2="38.1" y2="127" width="0.1524" layer="91"/>
<wire x1="38.1" y1="127" x2="58.42" y2="127" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="JP3" gate="A" pin="2"/>
<wire x1="203.2" y1="63.5" x2="198.12" y2="63.5" width="0.1524" layer="91"/>
<label x="198.12" y="63.5" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="GND" class="0">
<segment>
<pinref part="JP1" gate="A" pin="12"/>
<pinref part="GND1" gate="1" pin="GND"/>
<wire x1="66.04" y1="127" x2="76.2" y2="127" width="0.1524" layer="91"/>
<wire x1="76.2" y1="127" x2="76.2" y2="124.46" width="0.1524" layer="91"/>
</segment>
<segment>
<pinref part="JP2" gate="A" pin="8"/>
<wire x1="200.66" y1="124.46" x2="195.58" y2="124.46" width="0.1524" layer="91"/>
<label x="195.58" y="124.46" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
<segment>
<pinref part="JP3" gate="A" pin="1"/>
<wire x1="203.2" y1="66.04" x2="198.12" y2="66.04" width="0.1524" layer="91"/>
<label x="198.12" y="66.04" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="TX" class="0">
<segment>
<pinref part="JP1" gate="A" pin="8"/>
<wire x1="66.04" y1="132.08" x2="71.12" y2="132.08" width="0.1524" layer="91"/>
<label x="71.12" y="132.08" size="1.27" layer="95" xref="yes"/>
</segment>
<segment>
<pinref part="JP3" gate="A" pin="3"/>
<wire x1="203.2" y1="60.96" x2="198.12" y2="60.96" width="0.1524" layer="91"/>
<label x="198.12" y="60.96" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="JTAG_TDO" class="0">
<segment>
<pinref part="JP1" gate="A" pin="6"/>
<wire x1="66.04" y1="134.62" x2="71.12" y2="134.62" width="0.1524" layer="91"/>
<label x="71.12" y="134.62" size="1.27" layer="95" xref="yes"/>
</segment>
<segment>
<pinref part="JP2" gate="A" pin="4"/>
<wire x1="200.66" y1="134.62" x2="195.58" y2="134.62" width="0.1524" layer="91"/>
<label x="195.58" y="134.62" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="JTAG_TMS" class="0">
<segment>
<pinref part="JP1" gate="A" pin="4"/>
<wire x1="66.04" y1="137.16" x2="71.12" y2="137.16" width="0.1524" layer="91"/>
<label x="71.12" y="137.16" size="1.27" layer="95" xref="yes"/>
</segment>
<segment>
<pinref part="JP2" gate="A" pin="2"/>
<wire x1="200.66" y1="139.7" x2="195.58" y2="139.7" width="0.1524" layer="91"/>
<label x="195.58" y="139.7" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="JTAG_TCK" class="0">
<segment>
<pinref part="JP1" gate="A" pin="2"/>
<wire x1="66.04" y1="139.7" x2="71.12" y2="139.7" width="0.1524" layer="91"/>
<label x="71.12" y="139.7" size="1.27" layer="95" xref="yes"/>
</segment>
<segment>
<pinref part="JP2" gate="A" pin="3"/>
<wire x1="200.66" y1="137.16" x2="195.58" y2="137.16" width="0.1524" layer="91"/>
<label x="195.58" y="137.16" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="AD-EXT" class="0">
<segment>
<pinref part="JP1" gate="A" pin="1"/>
<wire x1="58.42" y1="139.7" x2="53.34" y2="139.7" width="0.1524" layer="91"/>
<label x="53.34" y="139.7" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
<segment>
<pinref part="JP3" gate="A" pin="9"/>
<wire x1="203.2" y1="45.72" x2="198.12" y2="45.72" width="0.1524" layer="91"/>
<label x="198.12" y="45.72" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="JTAG_TDI" class="0">
<segment>
<pinref part="JP1" gate="A" pin="3"/>
<wire x1="58.42" y1="137.16" x2="53.34" y2="137.16" width="0.1524" layer="91"/>
<label x="53.34" y="137.16" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
<segment>
<pinref part="JP2" gate="A" pin="5"/>
<wire x1="200.66" y1="132.08" x2="195.58" y2="132.08" width="0.1524" layer="91"/>
<label x="195.58" y="132.08" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="RX" class="0">
<segment>
<pinref part="JP1" gate="A" pin="5"/>
<wire x1="58.42" y1="134.62" x2="53.34" y2="134.62" width="0.1524" layer="91"/>
<label x="53.34" y="134.62" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
<segment>
<pinref part="JP3" gate="A" pin="7"/>
<wire x1="203.2" y1="50.8" x2="198.12" y2="50.8" width="0.1524" layer="91"/>
<label x="198.12" y="50.8" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="RESET" class="0">
<segment>
<pinref part="JP1" gate="A" pin="7"/>
<wire x1="58.42" y1="132.08" x2="53.34" y2="132.08" width="0.1524" layer="91"/>
<label x="53.34" y="132.08" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
<segment>
<pinref part="JP2" gate="A" pin="6"/>
<wire x1="200.66" y1="129.54" x2="195.58" y2="129.54" width="0.1524" layer="91"/>
<label x="195.58" y="129.54" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<wire x1="127" y1="175.26" x2="127" y2="5.08" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$2" class="0">
<segment>
<wire x1="5.08" y1="88.9" x2="256.54" y2="88.9" width="0.1524" layer="91"/>
</segment>
</net>
<net name="SDA-EXT" class="0">
<segment>
<pinref part="JP1" gate="A" pin="9"/>
<wire x1="58.42" y1="129.54" x2="53.34" y2="129.54" width="0.1524" layer="91"/>
<label x="53.34" y="129.54" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
<segment>
<pinref part="JP3" gate="A" pin="5"/>
<wire x1="203.2" y1="55.88" x2="198.12" y2="55.88" width="0.1524" layer="91"/>
<label x="198.12" y="55.88" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="SCL-EXT" class="0">
<segment>
<pinref part="JP1" gate="A" pin="10"/>
<wire x1="66.04" y1="129.54" x2="71.12" y2="129.54" width="0.1524" layer="91"/>
<label x="71.12" y="129.54" size="1.27" layer="95" xref="yes"/>
</segment>
<segment>
<pinref part="JP3" gate="A" pin="4"/>
<wire x1="203.2" y1="58.42" x2="198.12" y2="58.42" width="0.1524" layer="91"/>
<label x="198.12" y="58.42" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="JP3" gate="A" pin="6"/>
<wire x1="203.2" y1="53.34" x2="198.12" y2="53.34" width="0.1524" layer="91"/>
<label x="198.12" y="53.34" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
<net name="N$10" class="0">
<segment>
<pinref part="JP3" gate="A" pin="8"/>
<wire x1="203.2" y1="48.26" x2="198.12" y2="48.26" width="0.1524" layer="91"/>
<label x="198.12" y="48.26" size="1.27" layer="95" rot="MR0" xref="yes"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
