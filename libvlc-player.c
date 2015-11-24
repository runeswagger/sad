//use libvlc directly, because vlc can't consistantly bring up the rc interface
#include <vlc/vlc.h>

libvlc_instance_t *vlc;
libvlc_media_player_t *media_player;

void print_audio_outputs(libvlc_instance_t *vlc){
	//print available audio outputs
	libvlc_audio_output_t *outputs, *cur;

	outputs = libvlc_audio_output_list_get(vlc);

	cur = outputs;
	while(cur->p_next){
		printf("Name: %s\nDescription: %s\n\n", cur->psz_name, cur->psz_description);
		cur = cur->p_next;
	}

	libvlc_audio_output_list_release(outputs);
}

void print_audio_output_devices(libvlc_instance_t * vlc, const char * psz_name){
	//print available audio outputs
	libvlc_audio_output_device_t *devices, *cur;
	
	devices = libvlc_audio_output_device_list_get(vlc, psz_name);

	cur = devices;
	while(cur->p_next){
		printf("Device: %s\nDescription: %s\n\n", cur->psz_device, cur->psz_description);
		cur = cur->p_next;
	}
	
	libvlc_audio_output_device_list_release(devices);
}

void sad_vlc_exit_callback(void * data){
	//this cleans up the libvlc structures

}

int main(int argc, char *argv[]){
	//play a music file
	if (argc < 2) return -1;
	libvlc_media_t *media = NULL;

	//vlc setup
	vlc = libvlc_new(argc, (const char * const *) argv);
	media_player = libvlc_media_player_new(vlc);

	//audio setup
	libvlc_audio_output_device_set(media_player, "alsa", "default:CARD=Generic");

	//media open
	media = libvlc_media_new_path(vlc, argv[1]);
	libvlc_media_player_set_media(media_player, media);

	//media play
	libvlc_media_player_play(media_player);
	duration = libvlc_media_player_get_length(media_player);
	while(libvlc_media_player_get_position(media_player) < 1.0f) sleep(1);

	//vlc exit
	libvlc_media_player_release(media_player);
	libvlc_release(vlc);
	
	return 0;
}
