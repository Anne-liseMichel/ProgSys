#include "image.h"

int main(int arg_count, char **args)
{
	if(arg_count == 4)
	{
		int i;
		int status=0;
		char buf[2];
		int childPid, wpid;
		
		struct image input = make_image_from_file(args[1]);
		struct image output = make_image(input.type, input.row_count, input.column_count, input.max_value);
		
		/* On d�compose l'image en trois couleur (rouge, vert, bleu) et pour chaque couleur on effectue le traitement d'image
		 * Ce qui est int�ressant dans cette technique c'est que les traitements se font en parrall�le, on a donc un temps d'�xecution beaucoup plus court
		 */
		for(i=0;i<3;i++)
		{
			childPid=fork();
			if(childPid==0)
			{
				blur_image_layer(&input, &output, strtoul(args[3], 0, 0),i);
				sprintf(buf,"%d.ppm",i);
				write_image_to_file(&output, buf);
				exit(0);
			}
		}
		
		/*On attends que tous les fils aient fini de s'�xecuter pour continuer le traitement*/
		while ((wpid = wait(&status)) > 0)
		{
			
		}
		
		/*On cr�� l'image finale en combinant les trois fichiers*/
		for(i=0;i<3;i++)
		{
			sprintf(buf,"%d.ppm",i);
			input = make_image_from_file(buf);
			copy_image_layer(&input, &output, i);
		}
		
	
		write_image_to_file(&output, args[2]);
	}
	else
	{
		fprintf(stderr, "Essaie plut�t : %s input.ppm output.ppm 10\n", args[0]);
	}
}
