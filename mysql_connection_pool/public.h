#ifndef _PUBLIC_
#define _PUBLIC_

#define LOG(str) \
    cout << __FILE__ << ":" << __LINE__ << " " << __TIMESTAMP__ << " : " << str << endl;

#endif // !_PUBLIC_