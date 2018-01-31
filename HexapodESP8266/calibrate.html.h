// Calibration page - dynamic data
const char calib_html[] =  R"=====(<!DOCTYPE html>
<html>
<body>
<script type=text/javascript>
var xmlhttp=new XMLHttpRequest();
function update_status()
{ 
  if( document.getElementById("updt").checked )
  {
    xmlhttp.onreadystatechange=function()
    {
      if(xmlhttp.readyState==4&&xmlhttp.status==200)
      {
         var arr=JSON.parse(xmlhttp.responseText);
         document.getElementById("RRC_ANG").innerHTML=arr[0];
         document.getElementById("RRF_ANG").innerHTML=arr[1];
         document.getElementById("RRT_ANG").innerHTML=arr[2];
         document.getElementById("RCC_ANG").innerHTML=arr[3];
         document.getElementById("RCF_ANG").innerHTML=arr[4];
         document.getElementById("RCT_ANG").innerHTML=arr[5];
         document.getElementById("RFC_ANG").innerHTML=arr[6];
         document.getElementById("RFF_ANG").innerHTML=arr[7];
         document.getElementById("RFT_ANG").innerHTML=arr[8];
         document.getElementById("LRC_ANG").innerHTML=arr[9];
         document.getElementById("LRF_ANG").innerHTML=arr[10];
         document.getElementById("LRT_ANG").innerHTML=arr[11];
         document.getElementById("LCC_ANG").innerHTML=arr[12];
         document.getElementById("LCF_ANG").innerHTML=arr[13];
         document.getElementById("LCT_ANG").innerHTML=arr[14];
         document.getElementById("LFC_ANG").innerHTML=arr[15];
         document.getElementById("LFF_ANG").innerHTML=arr[16];
         document.getElementById("LFT_ANG").innerHTML=arr[17];
      }
    }
    xmlhttp.open("POST","/status",true);
    xmlhttp.send();
  }
}
</script>  
<H2>Calibration</H2>
<form action='#' method='post'>
<table width=80%>
<tr><td width=10%>Right Rear</td><th width=18%>Angle</th><th width=18%>Min Angle</th><th width=18%>Max Angle</th><th width=18%>PWM Center</th><th width=18%>PWM/900Deg</th></tr>
<tr><th>Coxa</th><td id='RRC_ANG'>%RRC_ANG%</td><td><input type='text' name='RRC_MINANG' value='%RRC_MINANG%'></td><td><input type='text' name='RRC_MAXANG' value='%RRC_MAXANG%'></td><td><input type='text' name='RRC_PWMC' value='%RRC_PWMC%'></td><td><input type='text' name='RRC_PWMDEG' value='%RRC_PWMDEG%'></td></tr>
<tr><th>Femur</th><td id='RRF_ANG'>%RRF_ANG%</td><td><input type='text' name='RRF_MINANG' value='%RRF_MINANG%'></td><td><input type='text' name='RRF_MAXANG' value='%RRF_MAXANG%'></td><td><input type='text' name='RRF_PWMC' value='%RRF_PWMC%'></td><td><input type='text' name='RRF_PWMDEG' value='%RRF_PWMDEG%'></td></tr>
<tr><th>Tibia</th><td id='RRT_ANG'>%RRT_ANG%</td><td><input type='text' name='RRT_MINANG' value='%RRT_MINANG%'></td><td><input type='text' name='RRT_MAXANG' value='%RRT_MAXANG%'></td><td><input type='text' name='RRT_PWMC' value='%RRT_PWMC%'></td><td><input type='text' name='RRT_PWMDEG' value='%RRT_PWMDEG%'></td></tr>

<tr><td>Right Center</td><th>Angle</th><th>Min Angle</th><th>Max Angle</th><th>PWM Center</th><th>PWM/900Deg</th></tr>
<tr><th>Coxa</th><td id='RCC_ANG'>%RCC_ANG%</td><td><input type='text' name='RCC_MINANG' value='%RCC_MINANG%'></td><td><input type='text' name='RCC_MAXANG' value='%RCC_MAXANG%'></td><td><input type='text' name='RCC_PWMC' value='%RCC_PWMC%'></td><td><input type='text' name='RCC_PWMDEG' value='%RCC_PWMDEG%'></td></tr>
<tr><th>Femur</th><td id='RCF_ANG'>%RCF_ANG%</td><td><input type='text' name='RCF_MINANG' value='%RCF_MINANG%'></td><td><input type='text' name='RCF_MAXANG' value='%RCF_MAXANG%'></td><td><input type='text' name='RCF_PWMC' value='%RCF_PWMC%'></td><td><input type='text' name='RCF_PWMDEG' value='%RCF_PWMDEG%'></td></tr>
<tr><th>Tibia</th><td id='RCT_ANG'>%RCT_ANG%</td><td><input type='text' name='RCT_MINANG' value='%RCT_MINANG%'></td><td><input type='text' name='RCT_MAXANG' value='%RCT_MAXANG%'></td><td><input type='text' name='RCT_PWMC' value='%RCT_PWMC%'></td><td><input type='text' name='RCT_PWMDEG' value='%RCT_PWMDEG%'></td></tr>

<tr><td>Right Front</td><th>Angle</th><th>Min Angle</th><th>Max Angle</th><th>PWM Center</th><th>PWM/900Deg</th></tr>
<tr><th>Coxa</th><td id='RFC_ANG'>%RFC_ANG%</td><td><input type='text' name='RFC_MINANG' value='%RFC_MINANG%'></td><td><input type='text' name='RFC_MAXANG' value='%RFC_MAXANG%'></td><td><input type='text' name='RFC_PWMC' value='%RFC_PWMC%'></td><td><input type='text' name='RFC_PWMDEG' value='%RFC_PWMDEG%'></td></tr>
<tr><th>Femur</th><td id='RFF_ANG'>%RFF_ANG%</td><td><input type='text' name='RFF_MINANG' value='%RFF_MINANG%'></td><td><input type='text' name='RFF_MAXANG' value='%RFF_MAXANG%'></td><td><input type='text' name='RFF_PWMC' value='%RFF_PWMC%'></td><td><input type='text' name='RFF_PWMDEG' value='%RFF_PWMDEG%'></td></tr>
<tr><th>Tibia</th><td id='RFT_ANG'>%RFT_ANG%</td><td><input type='text' name='RFT_MINANG' value='%RFT_MINANG%'></td><td><input type='text' name='RFT_MAXANG' value='%RFT_MAXANG%'></td><td><input type='text' name='RFT_PWMC' value='%RFT_PWMC%'></td><td><input type='text' name='RFT_PWMDEG' value='%RFT_PWMDEG%'></td></tr>

<tr><td>Left Rear</td><th>Angle</th><th>Min Angle</th><th>Max Angle</th><th>PWM Center</th><th>PWM/900Deg</th></tr>
<tr><th>Coxa</th><td id='LRC_ANG'>%LRC_ANG%</td><td><input type='text' name='LRC_MINANG' value='%LRC_MINANG%'></td><td><input type='text' name='LRC_MAXANG' value='%LRC_MAXANG%'></td><td><input type='text' name='LRC_PWMC' value='%LRC_PWMC%'></td><td><input type='text' name='LRC_PWMDEG' value='%LRC_PWMDEG%'></td></tr>
<tr><th>Femur</th><td id='LRF_ANG'>%LRF_ANG%</td><td><input type='text' name='LRF_MINANG' value='%LRF_MINANG%'></td><td><input type='text' name='LRF_MAXANG' value='%LRF_MAXANG%'></td><td><input type='text' name='LRF_PWMC' value='%LRF_PWMC%'></td><td><input type='text' name='LRF_PWMDEG' value='%LRF_PWMDEG%'></td></tr>
<tr><th>Tibia</th><td id='LRT_ANG'>%LRT_ANG%</td><td><input type='text' name='LRT_MINANG' value='%LRT_MINANG%'></td><td><input type='text' name='LRT_MAXANG' value='%LRT_MAXANG%'></td><td><input type='text' name='LRT_PWMC' value='%LRT_PWMC%'></td><td><input type='text' name='LRT_PWMDEG' value='%LRT_PWMDEG%'></td></tr>

<tr><td>Left Center</td><th>Angle</th><th>Min Angle</th><th>Max Angle</th><th>PWM Center</th><th>PWM/900Deg</th></tr>
<tr><th>Coxa</th><td id='LCC_ANG'>%LCC_ANG%</td><td><input type='text' name='LCC_MINANG' value='%LCC_MINANG%'></td><td><input type='text' name='LCC_MAXANG' value='%LCC_MAXANG%'></td><td><input type='text' name='LCC_PWMC' value='%LCC_PWMC%'></td><td><input type='text' name='LCC_PWMDEG' value='%LCC_PWMDEG%'></td></tr>
<tr><th>Femur</th><td id='LCF_ANG'>%LCF_ANG%</td><td><input type='text' name='LCF_MINANG' value='%LCF_MINANG%'></td><td><input type='text' name='LCF_MAXANG' value='%LCF_MAXANG%'></td><td><input type='text' name='LCF_PWMC' value='%LCF_PWMC%'></td><td><input type='text' name='LCF_PWMDEG' value='%LCF_PWMDEG%'></td></tr>
<tr><th>Tibia</th><td id='LCT_ANG'>%LCT_ANG%</td><td><input type='text' name='LCT_MINANG' value='%LCT_MINANG%'></td><td><input type='text' name='LCT_MAXANG' value='%LCT_MAXANG%'></td><td><input type='text' name='LCT_PWMC' value='%LCT_PWMC%'></td><td><input type='text' name='LCT_PWMDEG' value='%LCT_PWMDEG%'></td></tr>

<tr><td>Left Front</td><th>Angle</th><th>Min Angle</th><th>Max Angle</th><th>PWM Center</th><th>PWM/900Deg</th></tr>
<tr><th>Coxa</th><td id='LFC_ANG'>%LFC_ANG%</td><td><input type='text' name='LFC_MINANG' value='%LFC_MINANG%'></td><td><input type='text' name='LFC_MAXANG' value='%LFC_MAXANG%'></td><td><input type='text' name='LFC_PWMC' value='%LFC_PWMC%'></td><td><input type='text' name='LFC_PWMDEG' value='%LFC_PWMDEG%'></td></tr>
<tr><th>Femur</th><td id='LFF_ANG'>%LFF_ANG%</td><td><input type='text' name='LFF_MINANG' value='%LFF_MINANG%'></td><td><input type='text' name='LFF_MAXANG' value='%LFF_MAXANG%'></td><td><input type='text' name='LFF_PWMC' value='%LFF_PWMC%'></td><td><input type='text' name='LFF_PWMDEG' value='%LFF_PWMDEG%'></td></tr>
<tr><th>Tibia</th><td id='LFT_ANG'>%LFT_ANG%</td><td><input type='text' name='LFT_MINANG' value='%LFT_MINANG%'></td><td><input type='text' name='LFT_MAXANG' value='%LFT_MAXANG%'></td><td><input type='text' name='LFT_PWMC' value='%LFT_PWMC%'></td><td><input type='text' name='LFT_PWMDEG' value='%LFT_PWMDEG%'></td></tr>

</table>
<br/>
<input type='checkbox' id='updt'>Refresh
<button type='submit' name='move'>Move</button>&nbsp;
<button type='submit' name='load'>Load</button>&nbsp;
<button type='submit' name='save'>Save</button>&nbsp;
<button type='submit' name='set'>Set</button>&nbsp;
</form>
<script type=text/javascript>
setInterval(function(){ update_status(); },1000);
</script>      
</body>
</html>
)=====";
