$(document).ready(function(){
    var option = {
		dir:'/SyntaxHighlighter/', //required. path of SyntaxHighlighter
		/**
		 * Set SyntaxHighlighter default options on this page
		 * You can get more info at : 
		 * http://code.google.com/p/syntaxhighlighter/wiki/HighlightAll
		*/
		name:"code", //optional, default:"SyntaxHighlighter"
		showGutter:true, //optional, default:true
		showControls: false, //optional, default:false
		collapseAll:false, //optional, default:false
		firstLine : 1, //optional, default:false
		showColumns:false, //optional, default:false
		/**
		 * Options of this Plugin
		*/
		apptoall:false, //optional, default:true. enable default options to all elements instead of elements self option.
		autofind:true, //optional, default:true. auto enable highlighter to <pre> and <textarea> elements with 'class' attribute on this page whether with 'name'.
		jspath:'/SyntaxHighlighter/Scripts/', //optional, default:dir + 'Scripts/'. path of SyntaxHighlighter Js files
		csspath:'/SyntaxHighlighter/Styles/', //optional, default:dir + 'Styles/'. path of SyntaxHighlighter Css files
		swfpath:'/SyntaxHighlighter/Scripts/' //optional, default:dir + 'Scripts/' path of SyntaxHighlighter clipboard.swf file
	};
        jQuery.noConflict();
		jQuery.SyntaxHighlighter(option);
	});