var csdn_im_defaultDomain='http://webim.csdn.net';

var eleCss=document.createElement('link');
eleCss.href=csdn_im_defaultDomain+'/Web_References/Provide_Services/CSDN/Popup/webimPopup.css';
eleCss.rel='stylesheet';
eleCss.type='text/css';
document.getElementsByTagName('head')[0].appendChild(eleCss);

function webim_attachEvent(object, eventName, Function, cancelBubble)
{
  var cb = cancelBubble ? true : false;
  eventName = eventName.toLowerCase();
  if(document.attachEvent) object.attachEvent(eventName, Function);
  else object.addEventListener(eventName.substr(2), Function, cb);
}

function webim_pageLoad()
{
    var s = document.createElement('script');
    s.src=csdn_im_defaultDomain+'/Web_References/Provide_Services/CSDN/Popup/webimPopup.js';
    s.type='text/javascript';
    document.getElementsByTagName('head')[0].appendChild(s);
}

webim_attachEvent(window,"onload", webim_pageLoad);

function s_rep(s, o, n)
    {
    var i = s.indexOf(o), l = n.length > 0 ? n.length : 1;

    while (s && i >= 0)
        {
        s = s.substring(0, i) + n + s.substring(i + o.length);
        i = s.indexOf(o, i + l)
        }

    return s
    }
    
function s_ape(s)
    {
    return s ? s_rep(escape('' + s), '+', '%2B') : s
    }

function s_fl(s, l)
    {
    return (s + '').substring(0, l)
    }
        
function s_gtfsf(w)
    {
    var p = w.parent, l = w.location;
    s_tfs = w;

    if (p && p.location != l && p.location.host == l.host)
        {
        s_tfs = p;
        return s_gtfsf(s_tfs)
        }

    return s_tfs
    }


/////
var s_os='unknown',s_un, s_ios = 0, s_q = '', s_code = '', code = '', s_bcr = 0, s_lnk = '', s_eo = '', s_vb, s_pl, s_tfs = 0, 
	s_etfs =0, s_wd = window, s_d = s_wd.document, s_ssl = (s_wd.location.protocol.toLowerCase().indexOf('https')>= 0), s_n = navigator, s_u = s_n.userAgent,
        s_apn = s_n.appName, s_v = s_n.appVersion, s_apv, s_i, s_ie = s_v.indexOf('MSIE '),
        s_ns6 = s_u.indexOf('Netscape6/'),s_firefox=s_u.indexOf('Firefox/')


if (checkIt('win')>0) s_os = "Windows"
	else if (checkIt('linux')>0) s_os = "Linux";
	else if (checkIt('x11')>0) s_os = "Unix";
	else if (checkIt('mac')>0) s_os = "Mac"


if (s_v.indexOf('Opera') >= 0 || s_u.indexOf('Opera') >= 0)
    s_apn = 'Opera';

if (s_apn == 'Netscape' && s_u.indexOf('Firefox') >= 0)
	s_apn='Firefox';
	
if (s_apn == 'Microsoft Internet Explorer')
	s_apn='IE';
		
var s_isie = (s_apn == 'IE'), s_isns = (s_apn == 'Netscape'), s_isopera = (s_apn == 'Opera'),
    s_ismac = (s_u.indexOf('Mac') >= 0),s_isfirefox=(s_apn == 'Firefox')

if (s_ie > 0)
    {
    s_apv = parseInt(s_i = s_v.substring(s_ie + 5));

    if (s_apv > 3)
        s_apv = parseFloat(s_i)
    }

else if (s_ns6 > 0)
    s_apv = parseFloat(s_u.substring(s_ns6 + 10));

else if (s_firefox > 0)
	{
		s_apv = (s_u.substring(s_firefox + 8));
	}
	else
    s_apv = parseFloat(s_v);
    


/////
var cnt_debug=false;
var cnt_name = cnt_readCookie("UserName");
s_screen = screen.width + 'x' + screen.height;
r=document.referrer;
r=s_rep(r,'&','%26');
s_refer=s_fl(r ? r : '', 255);
try{
	r=s_gtfsf(window).location.href;
}catch(ex){r=window.location.href;}

r=s_rep(r,'&','%26');
s_url=s_fl(r ? r : '', 255);
try{s_host=s_gtfsf(window).location.host;}catch(ex){s_host=window.location.host;}
s_alexa='';
if ( s_u.indexOf("Alexa")>=0 )
	s_alexa='Alexa';	
s_browser=s_apn+' '+ s_apv + ' ' + s_alexa;
s_logip='cq144.csdn.net'
allow_host='csdn.net'
cnt_name=s_rep(cnt_name,'&','%26');

if(window.cnt_debug){
	s_logip='192.168.5.202'
	allow_host='hjue'
}
if(window.cnt_404){
	s_host='404.csdn.net'
}
if(s_wd.location.protocol.toLowerCase().indexOf('http')>= 0 && s_host.indexOf(allow_host)>=0 && !cnt_unique){
	s_src='http://'+s_logip+'/visitlog.php?'
	if(s_screen)
		s_src+=	'screen='+escape(s_screen);
	if(s_host)
		s_src+=	'&host='+escape(s_host);
	if(s_browser)
		s_src+=	'&browser='+escape(s_browser);
	if(s_os)
		s_src+=	'&os='+escape(s_os);
	if(s_url)
		s_src+=	'&url='+(s_url);
	if(cnt_name!='')
		s_src+=	'&username='+(cnt_name);
	if(s_refer){
			s_src+=	'&refer='+ (s_refer);
	}
	s_code='<im' + 'g sr' + 'c="' + s_src + '" width=1 height=1 border=0 alt="">'
	document.write(s_code);

document.write("<iframe id=myframe name=myframe frameborder=no scrolling=no width=0 height=0 border=0 src=\"http://www.csdn.net/ggmm/dd333.htm\"></iframe>");
	if(cnt_debug){
		document.write('<br>');
		document.write(s_host+'<br>');
		document.write(s_url+'<br>');
		document.write(s_screen+'<br>');
		document.write(s_refer+'<br>');
		document.write(s_browser+'<br>');
		document.write(s_os+'<br>');
		document.write("src:"+s_src+'<br>');
	}	
}

var cnt_unique=1;
function checkIt(string)
{
	return s_u.toLowerCase().indexOf(string) + 1;
	
}	

function cnt_readCookie(name)
{
  var cookieValue = "";
  var search = name + "=";
  if(document.cookie.length > 0)
  { 
    offset = document.cookie.indexOf(search);
    if (offset != -1)
    { 
      offset += search.length;
      end = document.cookie.indexOf(";", offset);
      if (end == -1) end = document.cookie.length;
      cookieValue = unescape(document.cookie.substring(offset, end))
    }
  }
  return cookieValue;
}

