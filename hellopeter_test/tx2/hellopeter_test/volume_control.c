#include "volume_control.h"

static snd_mixer_t * mixer;
static snd_mixer_elem_t *pcm_element;


//初始化------------------------------------------
void volume_init(void)
{
	snd_mixer_open(&mixer, 0);
	snd_mixer_attach(mixer, "default");
	snd_mixer_selem_register(mixer, NULL, NULL);
	snd_mixer_load(mixer);
	//找到Pcm对应的element,方法比较笨拙
	pcm_element = snd_mixer_first_elem(mixer);
	pcm_element = snd_mixer_elem_next(pcm_element);
	pcm_element = snd_mixer_elem_next(pcm_element);
	//
	long int a, b;
	long alsa_min_vol, alsa_max_vol;
	///处理alsa1.0之前的bug，之后的可略去该部分代码
	snd_mixer_selem_get_playback_volume(pcm_element, SND_MIXER_SCHN_FRONT_LEFT, &a);
	snd_mixer_selem_get_playback_volume(pcm_element, SND_MIXER_SCHN_FRONT_RIGHT, &b);

	snd_mixer_selem_get_playback_volume_range(pcm_element, &alsa_min_vol, &alsa_max_vol);
	///设定音量范围
	snd_mixer_selem_set_playback_volume_range(pcm_element, 0, 100);

}

//读音量值-----------------------------------------------------
int volume_read(void)
{
	long ll, lr;
	//处理事件
	snd_mixer_handle_events(mixer);
	//左声道
	snd_mixer_selem_get_playback_volume(pcm_element, SND_MIXER_SCHN_FRONT_LEFT, &ll);
	//右声道
	snd_mixer_selem_get_playback_volume(pcm_element,SND_MIXER_SCHN_FRONT_RIGHT, &lr);
	return (ll + lr) >> 1;
}

//写入音量-----------------------------------------------------
void volume_write(int leftright)
{
	//左音量
	snd_mixer_selem_set_playback_volume(pcm_element, SND_MIXER_SCHN_FRONT_LEFT, leftright);
	//右音量
	snd_mixer_selem_set_playback_volume(pcm_element, SND_MIXER_SCHN_FRONT_RIGHT, leftright);
}

//退出-----------------------------------------------------
void volume_uninit(void)
{
	snd_mixer_close(mixer);
}