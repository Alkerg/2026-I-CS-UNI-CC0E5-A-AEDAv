#include "linkedlist.h"

void AddOne(int& n){
    ++n;
}

void LinkedListDemo(){

    cout<<"----------LINKED LIST DEMO----------"<<endl;

    cout<<"LINKED LIST DESCENDENTE:"<<endl;
    LinkedList<DescendingLinkedListTrait<T1>> listDescending;
    listDescending.insert(7, 15);
    listDescending.insert(3, 25);
    listDescending.insert(9, 35);
    listDescending.insert(1, 45);
    cout<<listDescending<<endl;


    cout<<"LINKED LIST ASCENDENTE"<<endl;
    LinkedList<AscendingLinkedListTrait<T1>> listAscending;
    listAscending.insert(7, 15);
    listAscending.insert(3, 25);
    listAscending.insert(9, 35);
    listAscending.insert(1, 45);
    cout<<listAscending<<endl;

    cout<<"CONSTRUCTOR DE COPIA"<<endl;
    LinkedList<AscendingLinkedListTrait<T1>> listCopy(listAscending);
    cout<<listCopy<<endl;

    cout<<"CONSTRUCTOR DE MOVIMIENTO"<<endl;
    LinkedList<AscendingLinkedListTrait<T1>> listMove(move(listAscending));

    cout<<"Linked list transferido:"<<endl;
    cout<<listMove<<endl;
    cout<<"Linked list original despues de la transferencia:"<<endl;
    cout<<listAscending<<endl;
    
    cout<<"ASIGNACION DE COPIA"<<endl;
    LinkedList<DescendingLinkedListTrait<T1>> listCopyAssign;
    listCopyAssign = listDescending;
    cout<<listCopyAssign<<endl;

    cout<<"ASIGNACION DE MOVIMIENTO"<<endl;
    LinkedList<DescendingLinkedListTrait<T1>> listMoveAssign;
    listMoveAssign = move(listDescending);
    cout<<"Linked list transferido:"<<endl;
    cout<<listMoveAssign<<endl;
    cout<<"Linked list original despues de la transferencia:"<<endl;
    cout<<listDescending<<endl;

    cout<<"PUSH FRONT"<<endl;
    LinkedList<AscendingLinkedListTrait<T1>> newLinkedList;
    newLinkedList.insert(3, 15);
    newLinkedList.insert(2, 25);
    newLinkedList.insert(5, 35);
    cout<<newLinkedList<<endl;
    newLinkedList.push_front(0, 7);
    cout<<newLinkedList<<endl;

    cout<<"POP FRONT"<<endl;
    newLinkedList.pop_front();
    cout<<newLinkedList<<endl;

    cout<<"PUSH BACK"<<endl;
    newLinkedList.push_back(7, 58);
    cout<<newLinkedList<<endl;

    cout<<"POP BACK"<<endl;
    newLinkedList.pop_back();
    cout<<newLinkedList<<endl;

    cout<<"OPERATOR>>"<<endl;
    istringstream iss("[(1,100),(7,200),(6,150)]");
    LinkedList<DescendingLinkedListTrait<T1>> listFromStream;
    if(iss >> listFromStream){
        cout<<"Linked list leido:"<<listFromStream<<endl;
    } else {
        cout<<"Error al leer el linked list desde el stream."<<endl;
    }

    cout<<"FOR EACH"<<endl;
    cout<<"Linked list original: "<<newLinkedList<<endl;
    newLinkedList.ForEach(AddOne);
    cout<<"Linked list modificado: "<<newLinkedList<<endl;


    //cout<<listAscending[29]<<endl;
}

void ListsDemo(){
    LinkedListDemo();
    
}
