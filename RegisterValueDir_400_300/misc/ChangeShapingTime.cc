//
//   ChangeShapingTime.cc
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

const int N_Phrases = 3;
const std::string KeyPhrases[N_Phrases] = { 
  "Time Constant HG Shaper:", 
  "Time Constant LG Shaper:", 
  "(added by ChangeShapingTime)" 
};

int main( int argc, char **argv )
{
  std::string filepath;
  int shaping_t = 0;

  int opt;
  bool flag_ud[2] = { true, true };
  bool flag_hl[2] = { true, true };
  opterr = 0; // an extern variable in unistd.h
  while ( ( opt = getopt( argc, argv, "udhl" ) ) != -1 ) {
    switch ( opt ) {
    case 'u': 
      flag_ud[1] = false;
      break;
    case 'd': 
      flag_ud[0] = false;
      break;
    case 'h': 
      flag_hl[1] = false;
      break;
    case 'l': 
      flag_hl[0] = false;
      break;
    default: break;
    }
  }

  if ( argc - optind == 2 ) {
    filepath  = argv[optind];
    shaping_t = std::atoi( argv[optind + 1] );
    std::cout 
      << "[ChangeShapingTime] file name: " << filepath << "    "
      << "shaping time: " << shaping_t
      << std::endl;
  } else {
    std::cerr 
      << "[ChangeShapingTime] USAGE: [prompt]$ ./ChangeShapingTime <option> -- <file path> <shaping time>" << '\n'
      << std::endl;
      abort();
  }

  if ( shaping_t > 1023 || shaping_t < 0 ) {
    std::cerr 
      << "[ChangeShapingTime] ERROR: Too high/low shaping time of " << shaping_t << "."
      << std::endl;
    abort();
  }

  //// file reading ///////////////////////////////////////////////////////////
  std::ifstream infile( filepath.c_str() );
  std::vector< std::string > line;
  int done_udhl[2][2] = { { 0, 0 }, { 0, 0 } };
  if ( !infile.is_open() ) {
    std::cerr 
      << "[ChangeShapingTime] ERROR: Could not access the file \"" << filepath << "\"."
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
      } else if ( done_udhl[u_or_d][0] > 1 || done_udhl[u_or_d][1] > 1 ) {
	std::cerr 
	  << "[ChangeShapingTime] ERROR: Multiple Setting for EASIROC" << u_or_d + 1 << " was found"
	  << " at line " << i_line  << " in the file \"" << filepath << "\"."
	  << std::endl;
	abort();
      }
      if ( !flag_ud[u_or_d] ) {
	line.push_back( buff_line ); 
	continue; 
      }
      // key phrase seaching
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
      } // for ( i )

      if ( is_keyphrase.at( 2 ) ) { continue; } // in the case includes "(added by ChangeShapingTime)"
      bool flag_add = false;
      int  h_or_l   = -1;
      for ( int i = 0; i < 2; ++i ) {
	if ( flag_hl[i] && is_keyphrase.at( i ) ) {
	  h_or_l = ( i == 0 ) ? 0 : 1;
	  flag_add = true;
	}
      } // for (i)
      // additional line
      if ( flag_add ) {
	if ( done_udhl[u_or_d][h_or_l] ) { 
	  line.push_back( buff_line );
	  continue;
	} 
	if ( elem_line.at( 0 ) == "#" ) {
	  line.push_back( buff_line );
	} else {
	  line.push_back( "#" + buff_line );
	}
	std::stringstream ss;
	std::string hg_or_lg = ( h_or_l == 0 ) ? "HG" : "LG";
	ss << "  " << (keyphrase.at( h_or_l )).at( 0 ) << ' ' << (keyphrase.at( h_or_l )).at( 1 ) << ' '
	   << hg_or_lg << ' ' << (keyphrase.at( h_or_l )).at( 3 ) << ' '
	   << shaping_t << "ns"
	   << " (added by ChangeShapingTime)";
	line.push_back( ss.str() );
	++done_udhl[u_or_d][h_or_l];
      } else {
	line.push_back( buff_line );
      }
    } // while ( getline(...) )
  } // if ( !infile.is_open() )

  //// file writing ///////////////////////////////////////////////////////////
  if ( done_udhl[0][0] != 1 && done_udhl[0][1] != 1 && 
       done_udhl[1][0] != 1 && done_udhl[1][1] != 1 ) {
    std::cerr 
      << "[ChangeShapingTime] ERROR: Could not change shaping time."
      << std::endl;
    abort();
  } else {
    std::string outfile_name( filepath );
    std::ofstream outfile( outfile_name.c_str() );
    for ( std::vector< std::string >::iterator itr_line = line.begin(); itr_line != line.end(); ++itr_line ) {
      outfile << *itr_line << std::endl;
    }
    std::cout
      << "[ChangeShapingTime] The Register Value file \"" << outfile_name << "\" has been recreated."
      << std::endl;
  }

}
