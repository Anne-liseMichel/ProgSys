#include "image.h"

int main(int arg_count, char **args)
{
	if(arg_count == 4)
	{
		int i;
		int status=0;
		int childPid, wpid;
		FILE * tmp[3];
		int temps;
		
		struct image input = make_image_from_file(args[1]);
		struct image output = make_image(input.type, input.row_count, input.column_count, input.max_value);
		
		/* On décompose l'image en trois couleur (rouge, vert, bleu) et pour chaque couleur on effectue le traitement d'image
		 * Ce qui est intéressant dans cette technique c'est que les traitements se font en parrallèle, on a donc un temps d'éxecution beaucoup plus court
		 */
		 for(i=0;i<3;i++)
		{
			tmp[i]=tmpfile();
		}
		 
		for(i=0;i<3;i++)
		{
			childPid=fork();
			if(childPid<0)
			{
				abort();
			}
			else if(childPid==0)
			{
				blur_image_layer(&input, &output, strtoul(args[3], 0, 0),i);
				write_image_to_stream(&output, tmp[i]);
				exit(0);
			}
		}
		
		/*On attend que tous les fils aient fini de s'exécuter pour continuer le traitement*/
		i=3;
		while (i>0)
		{
			wpid = wait(&status);
			i--;
		}
		
		/*On créé l'image finale en combinant les trois fichiers*/
		for(i=0;i<3;i++)
		{
			fseek(tmp[i],0,SEEK_SET);
			input = make_image_from_stream(tmp[i]);
			copy_image_layer(&input, &output, i);
			fclose(tmp[i]);
		}
		
	
		write_image_to_file(&output, args[2]);
	}
	else
	{
		fprintf(stderr, "Essaie plutôt : %s input.ppm output.ppm 10\n", args[0]);
	}
}
