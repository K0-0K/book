function $(id) {
return document.getElementById(id);
}
function myRTrim(str, trimStr)
{
var s = new String(str);
if (trimStr.indexOf(s.charAt(s.length-1)) != -1)
{
var i = s.length - 1;
while (i >= 0 && trimStr.indexOf(s.charAt(i)) != -1){i--;}
s = s.substring(0, i+1);
}
return s;
}
var _FeedbackCountStack = "";
var _FeedbackCountResult = "";

function AddFeedbackCountStack(ID)
{
_FeedbackCountStack += ID + ",";
}
function LoadFeedbackCount()
{
SendRequest("/NewCount.aspx?FeedbackCountStack=" + _FeedbackCountStack);
}
var req;
function SendRequest(url)
{
if(window.XMLHttpRequest){
req = new XMLHttpRequest();
req.onreadystatechange = callback;
req.open("GET",url,true);
req.send(null);
}else if(window.ActiveXObject){
req=new ActiveXObject("Microsoft.XMLHTTP");
if(req){
req.onreadystatechange = callback;
req.open("GET",url,true);
req.send(null);
}
}
}
function callback(){
if(req.readyState==4){
if(req.status==200){
_FeedbackCountResult = req.responseText;
FillFeedbackCount();
}
}
}
function FillFeedbackCount()
{
if(_FeedbackCountResult == null || _FeedbackCountResult == "")
return;

var myFeedbackResultArray = _FeedbackCountResult.split(",");
var TmpResult;

for(var i=0; i<myFeedbackResultArray.length; i++)
{
TmpResult = myFeedbackResultArray[i].split("=");
$("FeedbackCount_" + TmpResult[0]).innerHTML = TmpResult[1];
}
}
function checkNull()
{
var searchScope = document.getElementById('Search_ddlSearchScope').value;
var searchText = document.getElementById('inputSearch').value;
if(searchText!='')
{
    var searchUri = "http://so.csdn.net/BlogSearchResult.aspx?q=" + searchText; ;
if(searchScope != 'all')
searchUri = searchUri + " username:"+searchScope;

window.open(searchUri);
}
return false;

}
function keyb(evt)
{

var eve = evt==null?window.event:evt;
if (eve.keyCode==13)
{
checkNull();
return false;
}
else return true;
}