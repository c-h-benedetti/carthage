
carthage = Carthage.init();

// In JavaScript, the accessor 'is_connected' scans for cookies and checks if the token didn't expire.

if (carthage.is_connected){
	projects = carthage.projects_list();
}
else{
	carthage.connection_page();
}

