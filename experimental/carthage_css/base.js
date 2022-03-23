'use strict';


const toolbox_items = [
	{
		'name': 'New',
		'icon': 'ri-add-circle-line'
	},
	{
		'name': 'Join Project',
		'icon': 'ri-link-m'
	},
	{
		'name': 'Favorites',
		'icon': 'ri-bookmark-3-line'
	},
	{
		'name': 'Recent',
		'icon': 'ri-time-line'
	},
	{
		'name': 'Processes',
		'icon': 'ri-task-line'
	},
	{
		'name': 'Jobs',
		'icon': 'ri-router-line'
	},
	{
		'name': 'Settings',
		'icon': 'ri-settings-4-line'
	},
];


const projects_names = [
	{
		'name': 'On A Railroad',
		'thumbnail': 'bg1.jpg'
	},
	{
		'name': 'Abstract Project',
		'thumbnail': 'wtf.png'
	},
	{
		'name': 'On The Road By Night',
		'thumbnail': 'bg3.jpg'
	},
	{
		'name': 'Deep In The Forest',
		'thumbnail': 'bg4.jpg'
	},
	{
		'name': 'The Siren',
		'thumbnail': 'bg2.jpg'
	},
	{
		'name': 'Above All',
		'thumbnail': 'bg6.jpg'
	},
	{
		'name': 'Somewhere On Earth',
		'thumbnail': 'bg7.jpg'
	},
	{
		'name': 'Before Paris',
		'thumbnail': 'bg8.jpg'
	},
	{
		'name': 'An Old Restaurant',
		'thumbnail': 'bg9.jpg'
	},
	{
		'name': 'Ancient Warships',
		'thumbnail': 'bg10.webp'
	},
	{
		'name': 'Exploring Poveglia',
		'thumbnail': 'bg11.jpeg'
	},
	{
		'name': 'Lost In The South',
		'thumbnail': 'bg12.jpg'
	},
	{
		'name': 'Los Angeles Explorators',
		'thumbnail': 'bg13.jpg'
	},
	{
		'name': 'Above All',
		'thumbnail': 'bg6.jpg'
	},
	{
		'name': 'Somewhere On Earth',
		'thumbnail': 'bg7.jpg'
	},
	{
		'name': 'Before Paris',
		'thumbnail': 'bg8.jpg'
	},
	{
		'name': 'An Old Restaurant',
		'thumbnail': 'bg9.jpg'
	},
	{
		'name': 'Ancient Warships',
		'thumbnail': 'bg10.webp'
	},
	{
		'name': 'Exploring Poveglia',
		'thumbnail': 'bg11.jpeg'
	},
	{
		'name': 'Lost In The South',
		'thumbnail': 'bg12.jpg'
	},
	{
		'name': 'Los Angeles Explorators',
		'thumbnail': 'bg13.jpg'
	}
];


class SearchBar extends React.Component{
	constructor(props){
		super(props);
		this.state = { 
			text: "",
			icon: "ri-search-eye-line"
		};
	}

	search_button = () => {
		const search_box = document.querySelector('#search_box');
		alert(search_box.value);
	};

	render(){
		return (
			<div id="search">
				<i className={this.state.icon} onClick={this.search_button} id="ico_search_bar" title="Launch searching process!"></i>
				<input type="text" id="search_box" placeholder="I'm searching for..."/>
			</div>
		);
	}
}


class TopBanner extends React.Component{
	constructor(props){
		super(props);
		this.state = {};
	}

	render(){
		return (
			<div id="banner">
				<img src="./media/carthage-logo.png" alt="Carthage's Logo" id="logo_carthage"/>
				<SearchBar/>
			</div>
		);
	}
}


function generate_menu_items(){
	let items = [];

	for (let i = 0 ; i < toolbox_items.length ; i++){
		items[i] = (
			<div key={"menu_item"+i} className="menu_item">
				<span className={toolbox_items[i].icon + " menu_icon"}></span>
				<span className="menu_text">{toolbox_items[i].name}</span>
			</div>
		);
	}

	return items;
}


function generate_connection_status(connected){
	if (connected){
		return(
			<div id="connexion">
				<span className="ri-cloud-line connexion_on menu_icon"></span>
				<span className="connexion_text">α1.0.0</span>
			</div>
		);
	}
	else{
		return(
			<div id="connexion">
				<span className="ri-cloud-off-line connexion_off menu_icon"></span>
				<span className="connexion_text">α1.0.0</span>
			</div>
		);
	}
}

const switch_star = (obj) => {

	if (obj.target.classList.value == "star_pjct ri-star-line"){
		obj.target.classList.value = "star_pjct ri-star-fill"
	}
	else{
		obj.target.classList.value = "star_pjct ri-star-line"
	}
};

function generate_temp_projects(){
	let items = [];

	for (let i = 0 ; i < projects_names.length ; i++){
		items[i] = (
			<div className="pjt_container" key={"pjt_container"+i}>
				<i onClick={switch_star} className="star_pjct ri-star-line"></i>
				<div className="pjt_stripe">
					<img className="pjt_img" src={"media/thumbnails/"+projects_names[i].thumbnail}/>
				</div>
				<p className="pjt_name">{projects_names[i].name}</p>
			</div>
		);
	}

	return items;
}


class LeftPanel extends React.Component{

}

class MainSection extends React.Component{

}



function main(){
    const bandeau = document.querySelector('#bandeau');
    ReactDOM.render(React.createElement(TopBanner), bandeau);

    const left_tools = document.querySelector('#left_tools');
    ReactDOM.render(generate_menu_items(), left_tools);

    const connection = document.querySelector('#connection_status');
    ReactDOM.render(generate_connection_status(false), connection);

    const main_block = document.querySelector('#main_block');
    ReactDOM.render(generate_temp_projects(), main_block);
}


main();


// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
// =    TESTING                                                    =
// = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =


class LikeButton extends React.Component {
	constructor(props) {
		super(props);
		this.state = { liked: false };
	}

	render() {
		if (this.state.liked) {
			return 'You liked this.';
		}

		return e(
			'button',
			{ onClick: () => this.setState({ liked: true }) },
			'Like'
		);
	}
}