function GetElement(id,tag)
{
	var ret=document.getElementById(id);

	if(tag)
		return ret.getElementsByTagName(tag);
	else
		return ret;
}


function slide()
{
	var root=GetElement("slideimg","");	
	var sub=GetElement("slideimg","dl");
   	var SlideWidth=sub[0].offsetWidth;		 
	var index=0;					
	var scrollwidth=SlideWidth-4;
	var myTimer;
	var SlidePic=GetElement("navig","img");

	root.scrollLeft=scrollwidth+(index*SlideWidth);

	for(var i=0;i<SlidePic.length;i++){
		regOnClick(i);
	};



	function regOnClick(i){
		SlidePic[i].onclick=function(){
		sub[index].className=SlidePic[index].className="";
		index=i;
		sub[index].className=SlidePic[index].className="show";
		myTimer=setInterval(change,5);
		}
	}


	function change()
	{
		var space=((index*SlideWidth+scrollwidth)-root.scrollLeft);
		var v=space>0?Math.ceil(space/10):Math.floor(space/10);
		root.scrollLeft+=v;
		if(v==0){
			clearInterval(myTimer);
			sub[index].className=SlidePic[index].className="show";
			v=null;
		}
	}
}



