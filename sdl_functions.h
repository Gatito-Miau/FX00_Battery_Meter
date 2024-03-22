#ifndef SDL_FUNCTIONS_H
#define SDL_FUNCTIONS_H

SDL_Surface *load_image( std::string filename ) { 

	SDL_Surface* loadedImage = NULL; 

	SDL_Surface* optimizedImage = NULL; 

	loadedImage = IMG_Load( filename.c_str() ); 

	if( loadedImage != NULL ) {
		optimizedImage = SDL_DisplayFormatAlpha( loadedImage ); 
		SDL_FreeSurface( loadedImage ); 
	} 
	return optimizedImage;
} 

#endif
