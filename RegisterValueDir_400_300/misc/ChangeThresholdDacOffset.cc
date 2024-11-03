//
//   ChangeThresholdDacOffset.cc
//
// Jan. 16, 2016, Y.Nakada
//


// This program add common offset to each threshold DAC value.
//
// instruction of options
//       -a : DAC values of the all channels are changed corresponding to the offset value.
//       -u : DAC values of the upper channels (0--31) are changed corresponding to the offset value.
//       -d : DAC values of the lower channels (32--63) are changed corresponding to the offset value.
//       -i<ch1> -f<ch2> :  DAC values of the range <ch1> to <ch2> are changed corresponding to the offset value.
//       --  : This expression MUST be writen in the end of the option array. If this option absent, a negative offset is regarded as an option.


#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>
#include <algorithm>

const int N_Phrases = 4;
const std::string KeyPhrases[N_Phrases] = { 
  "DAC code:", 
  "base line", 
  "baseline", 
  "(added by ChangeThresholdDacOffset)" 
};
//const std::string str2 = "base line";
//const std::string str3 = "baseline";
//const std::string str4 = "(added by ChangeThresholdDacOffset)";

int main( int argc, char **argv )
{
  std::string filepath;
  int offset = 0;

  int opt;
  bool flag_ud[2] = { true, true };
  opterr = 0; // an extern variable in unistd.h
  while ( ( opt = getopt( argc, argv, "ud" ) ) != -1 ) {
    switch ( opt ) {
    case 'u': 
      flag_ud[1] = false;
      break;
    case 'd': 
      flag_ud[0] = false;
      break;
    default: break;
    }
  }

  if ( argc - optind == 2 ) {
    filepath = argv[optind];
    offset   = std::atoi( argv[optind + 1] );
    std::cout 
      << "[ChangeThresholdDacOffset] file name: " << filepath << "    "
      << "offset value: " << offset 
      << std::endl;
  } else {
    std::cerr 
      << "[ChangeThresholdDacOffset] USAGE: [prompt]$ ./ChangeThresholdDacOffset <option> -- <file path> <offset value>" << '\n'
      << std::endl;
      abort();
  }

  if ( abs( offset ) > 1023 ) {
    std::cerr 
      << "[ChangeThresholdDacOffset] ERROR: Too high/low offset of " << offset << "."
      << std::endl;
    abort();
  }

  //// file reading ///////////////////////////////////////////////////////////
  std::ifstream infile( filepath.c_str() );
  std::vector< std::string > line;
  int done_ud[2] = { 0, 0 };
  if ( !infile.is_open() ) {
    std::cerr 
      << "[ChangeThresholdDacOffset] ERROR: Could not access the file \"" << filepath << "\"."
      << std::endl;
    abort();
  } else {
    std::vector< std::vector< std::string > > keyphrase;
    for ( int i = 0; i < N_Phrases; ++i ) {
      std::vector< std::string > vec_str;
      std::stringstream ss( KeyPhrases[i] );
      std::string str_tmp;
      while ( ss >> str_tmp ) {
	vec_str.push_back( str_tmp );
      }
      keyphrase.push_back( vec_str );
    }

    int i_line = 0;
    std::string buff_line;
    int u_or_d = -1;
    while ( getline( infile, buff_line ) ) {
      i_line++;
      std::stringstream ss( buff_line );
      std::string buff;
      std::vector< std::string > elem_line;
      while ( ss >> buff ) {
	  elem_line.push_back( buff );
      }
      if ( elem_line.size() == 0 ) {
	line.push_back( buff_line );
	continue;
      }
      if ( elem_line.at( 0 ) == "EASIROC1:" ) {
	u_or_d = 0;
	line.push_back( buff_line );
	continue;
      } else if ( elem_line.at( 0 ) == "EASIROC2:" ) {
	u_or_d = 1;
	line.push_back( buff_line );
	continue;
      }
      if ( u_or_d == -1 ) { 
	line.push_back( buff_line ); 
	continue; 
      } else if ( done_ud[u_or_d] > 1 ) {
	std::cerr 
	  << "[ChangeThresholdDacOffset] ERROR: Multiple Setting for EASIROC" << u_or_d + 1 << " was found"
	  << " at line " << i_line  << " in the file \"" << filepath << "\"."
	  << std::endl;
	abort();
      }
      if ( !flag_ud[u_or_d] ) {
	line.push_back( buff_line ); 
	continue; 
      }

      int dac = -1, val = 0;
      std::vector< bool > is_keyphrase( N_Phrases, false );
      std::vector< std::string >::iterator itr_begin = elem_line.begin();
      for ( int i = 0; i < N_Phrases; ++i ) {
	std::vector< std::string >::iterator itr_elem_line = std::find( itr_begin, elem_line.end(), (keyphrase.at( i )).at( 0 ) );
	if ( itr_elem_line == elem_line.end() ) {
	  continue;
	}
	int n_words = (keyphrase.at( i )).size();
	int remainder = elem_line.end() - itr_elem_line;
	if ( remainder < n_words ) {
	  continue; 
	} else if ( remainder == 1 ) {
	    is_keyphrase.at( i ) = true;
	} else {
	  for ( int j = 1; j < n_words; ++j ) {
	    if ( *(itr_elem_line + 1) != (keyphrase.at( i )).at( j ) ) {
	      break;
	    } else if ( j == n_words - 1 ) {
	      is_keyphrase.at( i ) = true;
	      itr_begin = itr_elem_line + 1;
	    }
	    ++itr_elem_line;
	  } // for ( j )
	}
	
	if ( i == 0 && is_keyphrase.at( i ) && remainder - n_words > 0 ) {
	  dac = std::atoi( (*(itr_elem_line + n_words - 1)).c_str() );
	}
	if ( ( i == 1 || i == 2 ) && is_keyphrase.at( i ) && remainder - n_words > 0 ) {
	  val = std::atoi( (*(itr_elem_line + n_words - 1)).c_str() );
	}
      } // for ( i )
      //      std::cout << i_line << ": " << is_keyphrase.at( 0 ) << '\t' << is_keyphrase.at( 1 ) << '\t' << is_keyphrase.at( 2 ) << '\t' << is_keyphrase.at( 3 ) << std::endl;
      
      if ( is_keyphrase.at( 3 ) ) { continue; } // in the case includes "(added by ChangeThresholdDacOffset)"
      if ( is_keyphrase.at( 0 ) ) { // in the case includes "DAC code:"
	if ( elem_line.at( 0 ) == (keyphrase.at( 0 )).at( 0 ) && elem_line.at( 1 ) == (keyphrase.at( 0 )).at( 1 ) ) { // in the case that first word is "DAC"
	  line.push_back( "#" + buff_line );
	} else {
	  line.push_back( buff_line );
	}
	// additional line
	if ( ( is_keyphrase.at( 1 ) || is_keyphrase.at( 2 ) ) && val == 0 ) { // in the case includes "base line" or "baseline"
	  if ( done_ud[u_or_d] > 0 ) {
	    std::cerr 
	      << "[ChangeThresholdDacOffset] ERROR: Multiple base line setting for EASIROC" << u_or_d + 1 << " was found"
	      << " at line " << i_line << " in the file \"" << filepath << "\"."
	      << std::endl;
	    abort();
	  }
	  int dac_new = dac + offset;
	  if ( dac_new > 1024 || dac_new < 0 ) {
	    std::cerr 
	      << "[ChangeThresholdDacOffset] ERROR: The new threshold DAC value of " << dac << " + " << offset
	      << " at line " << i_line << " exceeded the allowed range of 0 to 1023." << "\n"
	      << "[ChangeThresholdDacOffset] The new input DAC value ramains the original one."
	      << std::endl;
	    line.push_back( buff_line );
	    continue;
	  }
	  std::stringstream ss;
	  if ( offset < 0 ) {
	    ss << "  " << (keyphrase.at( 0 )).at( 0 ) << ' ' << (keyphrase.at( 0 )).at( 1 ) << ' ' << dac_new
	       << " # baseline " << offset << " (added by ChangeThresholdDacOffset)";
	  } else {
	    ss << "  " << (keyphrase.at( 0 )).at( 0 ) << ' ' << (keyphrase.at( 0 )).at( 1 ) << ' ' << dac_new
	       << " # baseline " << '+' << offset << " (added by ChangeThresholdDacOffset)";
	  }
	  line.push_back( ss.str() );
	  ++done_ud[u_or_d];
	}
      } else {
	line.push_back( buff_line );
      }
    } // while ( getline(...) )
  } // if ( !infile.is_open() )

  //// file writing ///////////////////////////////////////////////////////////
  if ( done_ud[0] != 1 && done_ud[1] != 1 ) {
    std::cerr 
      << "[ChangeThresholdDacOffset] ERROR: Could not calculate appropiate DAC value."
      << std::endl;
    abort();
  } else {
    std::string outfile_name( filepath );
    std::ofstream outfile( outfile_name.c_str() );
    for ( std::vector< std::string >::iterator itr_line = line.begin(); itr_line != line.end(); ++itr_line ) {
      outfile << *itr_line << std::endl;
    }
    std::cout
      << "[ChangeThresholdDacOffset] The Register Value file \"" << outfile_name << "\" has been recreated."
      << std::endl;
  }
  

}
