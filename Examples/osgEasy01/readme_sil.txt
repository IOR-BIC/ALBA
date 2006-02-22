esempio di un Semplice Manipolatore:

 1) derivare il proprio manipolatore da MatrixManipulator
 
 2) implementare 
      GetMatrix
      GetInverseMatrix
      SetByMatrix
      SetByInverseMatrix
      
    Qualcosa fuori dal manipolatore chiamera' questi metodi
    ed applichera la Matrice alla Camera - la Camera non e' raggiungibile direttamente
    
    Le SetXXX vengono chiamate quando si Switcha da un manipolatore ad un altro,
    in modo che il nuovo manipolatore si posizioni (se possibile) come il precedente
    
 3) Implementare 
       SetNode
       Home
       
    Home posiziona la camera in una posizione di default,
    tipicamente a vedere tutto il dato eventualmente gia specificato con SetNode
    
    
 4) Implementare 
        bool Handle(ea,us)            
        
    La Handle e' dove si ricevono gli eventi,
    in risposta ai quali di deve modificare la Matrice della camera.
    
    ea e' una astrazione di evento (cioe' un evento non dipendente dall'OS )
    Ha dei metodi tipo: GetEventType, GetKey, GetMousePosition, ....
    
    us e' una astrazione di Viewer (cioe' un viewer non dipendente dall'OS e sconosciuto )
    us hai dei metodi tipo 
      RequestUpdate()               -- chiedi il refresh della finestra
      RequestContinousUpdate(bool), -- continua a mandarmi eventi, se necessario creandoti un timer 
    
    Handle deve ritornare un Bool che e' true se l'evento e' stato consumato,
    se handle ritorna false, l'evento viene passato anche agli altri manipolatori attivi.
    
