#include "fs.h"
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

char a[5476] =
    "Once upon a time, in the small village of Eldoria, nestled between "
    "rolling hills and dense forests, lived an old clockmaker named Elias. His "
    "workshop, situated at the heart of the village, was a place of wonder and "
    "mystery. Intricate gears, springs, and clock faces adorned every shelf "
    "and corner, creating a mesmerizing labyrinth of timepieces. Elias was "
    "renowned for his skills, and people from distant lands sought his "
    "expertise to mend their broken clocks and watches. One crisp autumn "
    "morning, a stranger arrived in Eldoria. She was a young woman named "
    "Elara, with eyes that sparkled like the morning dew and an air of quiet "
    "determination. Clutched tightly in her hands was an ornate pocket watch, "
    "its silver surface etched with intricate designs and runes that seemed to "
    "dance in the light. Elara's expression was one of both hope and "
    "trepidation as she entered Elias's workshop. \"Good morning,\" she "
    "greeted softly, her voice tinged with an unfamiliar accent. \"I was told "
    "you are the best clockmaker in the land. I need your help with this "
    "watch.\" Elias looked up from his workbench, his eyes narrowing slightly "
    "as he took in the sight of the pocket watch. He could immediately sense "
    "something extraordinary about it, something almost otherworldly. He "
    "gestured for Elara to come closer and took the watch from her hands, "
    "feeling a subtle warmth emanating from its surface. \"Where did you get "
    "this?\" he asked, his voice a mix of curiosity and caution. \"It belonged "
    "to my grandfather,\" Elara explained. \"He was an explorer and found it "
    "in the ruins of an ancient temple. Before he passed away, he told me it "
    "held great power, but it stopped working many years ago. I don't know "
    "what to do, but I believe it’s important.\" Elias nodded, his fingers "
    "tracing the delicate engravings on the watch. \"This is no ordinary "
    "timepiece,\" he murmured. \"I can sense a magical aura around it. I will "
    "do my best to repair it, but it may take some time.\" Elara's eyes lit up "
    "with gratitude. \"Thank you, Elias. I will wait as long as it takes.\" "
    "For the next several weeks, Elias immersed himself in the task of "
    "repairing the pocket watch. He meticulously examined every gear and "
    "spring, consulting ancient tomes and scrolls that lined the dusty shelves "
    "of his workshop. The more he delved into its workings, the more he "
    "realized the watch was far more than a mere timekeeping device. It seemed "
    "to have a connection to the very fabric of time itself. One evening, as "
    "Elias was adjusting the final spring, a sudden flash of light filled the "
    "workshop. The watch began to glow with an ethereal blue light, and the "
    "runes on its surface pulsed with energy. Elias stepped back, shielding "
    "his eyes, and watched in awe as the watch levitated above the workbench, "
    "spinning slowly. A voice, soft and melodic, echoed through the room. "
    "\"Elias, guardian of timepieces, you have awakened the Chronomancer's "
    "Watch.\"Elias stared in astonishment as the voice continued. \"This watch "
    "holds the power to traverse the streams of time, to witness the past and "
    "glimpse the future. But beware, for such power comes with great "
    "responsibility. The balance of time must be preserved.\" As the light "
    "dimmed, the watch settled back onto the workbench, its glow fading to a "
    "soft shimmer. Elias approached it cautiously, his mind racing with "
    "questions. He knew that he needed to understand the full extent of its "
    "power before he could hand it back to Elara. That night, Elias had a "
    "vivid dream. He found himself standing in a vast, starry void, with "
    "streams of light flowing around him like rivers. A figure appeared before "
    "him, cloaked in shadows, with eyes that glowed like distant stars. \"Who "
    "are you?\" Elias asked, his voice echoing in the emptiness. \"I am the "
    "Keeper of Time,\" the figure replied. \"The Chronomancer's Watch is a "
    "tool created by ancient sorcerers to safeguard the timeline. It can show "
    "glimpses of what was and what could be. But it must never be used to "
    "alter the past or the future, for such actions could unravel the fabric "
    "of reality itself.\" Elias woke with a start, the Keeper's warning "
    "echoing in his mind. He knew that the watch was a powerful artifact that "
    "needed to be handled with the utmost care. The next morning, he called "
    "Elara to the workshop. \"I have repaired the watch,\" he said, holding it "
    "out to her. \"But you must understand, this is no ordinary watch. It has "
    "the power to traverse time, to see the past and the future. You must use "
    "it wisely and never attempt to change what you witness.\" Elara's eyes "
    "widened in astonishment. \"I had no idea it was so powerful. Thank you, "
    "Elias. I promise to use it responsibly.\" Elias watched as Elara took the "
    "watch, her hands trembling slightly. He could see the determination in "
    "her eyes and knew that she was destined for great things. As she left the "
    "workshop, Elias felt a sense of fulfillment, knowing that he had played a "
    "part in preserving the balance of time. Over the years, Elara's "
    "adventures with the Chronomancer's Watch became the stuff of legend. She "
    "used its power to uncover lost histories, prevent disasters, and guide "
    "her people toward a brighter future. And through it all, she never forgot "
    "the wise clockmaker who had entrusted her with such a precious gift. In "
    "Eldoria, the seasons changed, and life went on. But in the heart of the "
    "village, the old clockmaker's workshop remained a place of wonder, where "
    "the ticking of clocks was a reminder that time, in all its mystery and "
    "magic, was both a gift and a responsibility.";

// TODO: memcpy() could be avoided in most of cases. unions could be used.
int main() {
  FileSystem fs;
  Disk *disk = disk_open("file2", 20);
  char buf[4096];

  fs_format(&fs, disk);
  /* fs_debug(fs.disk); */

  int inode = fs_create(&fs);
  printf("%d\n", inode);

  int inode2 = fs_create(&fs);
  printf("%d\n", inode2);

  int inode3 = fs_create(&fs);
  printf("%d\n", inode3);

  srand(time(NULL));
  char save[5000];
  for (int i = 0; i < 5000; i++) {
    save[i] = 'A' + rand() % 26;
  }

  fs_write(&fs, 255, a, 5476, 0);

  char rbuf[5476];
  fs_read(&fs, 255, rbuf, 5476, 0);

  /* for (int i = 0; i < 8185; i++) { */
  for (int i = 0; i < 5476; i++) {
    printf("%c", rbuf[i]);
  }
  /* fs_debug(fs.disk); */

  disk_close(fs.disk);
}
